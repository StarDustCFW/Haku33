// Helpers for the save data indexer database (imkvdb.arc) stored in system
// save 8000000000000000.
//
// File layout (little endian):
//   "IMKV" | u32 reserved | u32 entry_count
//   entry_count x { "IMEN" | u32 key_size | u32 value_size | key | value }
// For save data the key is a 0x40 byte FsSaveDataAttribute and the value is
// 0x40 bytes starting with u64 save_data_id, u64 size.
//
// This header has no libnx dependency on purpose, so it can be compiled and
// tested on a PC.
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace imkvdb {

static const uint64_t SaveNcmSystem      = 0x8000000000000120ULL; // content meta database, needed to boot
static const uint64_t SaveDeviceSettings = 0x8000000000000053ULL; // battery calibration, must survive a wipe

static inline uint32_t rd32(const uint8_t *p) {
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline uint64_t rd64(const uint8_t *p) {
    return (uint64_t)rd32(p) | ((uint64_t)rd32(p + 4) << 32);
}

static inline void wr32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)v; p[1] = (uint8_t)(v >> 8); p[2] = (uint8_t)(v >> 16); p[3] = (uint8_t)(v >> 24);
}

// True for the index records that have to stay after a wipe: the system
// (non user, non application) saves 8000000000000120 and 8000000000000053.
static inline bool IsKeptRecord(const uint8_t *key, uint32_t key_size, uint32_t value_size) {
    if (key_size != 0x40 || value_size != 0x40) return false;
    static const uint8_t zero[0x18] = {0};
    if (memcmp(key, zero, sizeof(zero)) != 0) return false;   // application_id + uid must be 0
    if (key[0x20] != 0) return false;                         // save_data_type must be System
    const uint64_t id = rd64(key + 0x18);                     // system_save_data_id
    return id == SaveNcmSystem || id == SaveDeviceSettings;
}

// Builds the index a freshly wiped console needs out of the console's own
// index: the records for 8000000000000120 and 8000000000000053 are copied
// unchanged (same order, same sizes), everything else is dropped.
// This is what romfs/startup.te builds from TegraExplorer, without guessing
// any save size.
//
// Returns false, leaving `out` empty, if `in` is not a well formed index or
// has no record for 8000000000000120. `kept53` (optional) reports whether the
// 8000000000000053 record was present.
static inline bool BuildCleanIndex(const std::vector<uint8_t> &in, std::vector<uint8_t> &out, bool *kept53 = nullptr) {
    out.clear();
    if (kept53) *kept53 = false;

    const size_t header_size = 12, entry_header_size = 12;
    if (in.size() < header_size || memcmp(in.data(), "IMKV", 4) != 0) return false;

    const uint32_t count = rd32(in.data() + 8);
    std::vector<uint8_t> body;
    uint32_t kept = 0;
    bool has120 = false, has53 = false;
    size_t off = header_size;

    for (uint32_t i = 0; i < count; i++) {
        if (in.size() - off < entry_header_size) return false;
        const uint8_t *e = in.data() + off;
        if (memcmp(e, "IMEN", 4) != 0) return false;

        const uint32_t key_size = rd32(e + 4), value_size = rd32(e + 8);
        const size_t remaining = in.size() - off - entry_header_size;
        if (key_size > remaining || value_size > remaining - key_size) return false;
        const size_t entry_size = entry_header_size + key_size + value_size;

        if (IsKeptRecord(e + entry_header_size, key_size, value_size)) {
            const uint64_t id = rd64(e + entry_header_size + 0x18);
            bool &seen = (id == SaveNcmSystem) ? has120 : has53;
            if (!seen) {
                seen = true;
                body.insert(body.end(), e, e + entry_size);
                kept++;
            }
        }
        off += entry_size;
    }

    if (!has120) return false;

    out.assign(in.begin(), in.begin() + header_size);
    wr32(out.data() + 8, kept);
    out.insert(out.end(), body.begin(), body.end());
    if (kept53) *kept53 = has53;
    return true;
}

} // namespace imkvdb
