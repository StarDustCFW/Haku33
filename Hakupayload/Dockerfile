# Build Docker image using: docker build . -t argon-nx-builder
# Run docker image using: docker run -a stdout -a stderr -v $(pwd)/output:/argon-nx/output argon-nx-builder

FROM devkitpro/devkitarm
WORKDIR /argon-nx
COPY . .
ENTRYPOINT ["make"]