# LucidLogging


## Getting Started
These instructions will guide you through setting up LucidLogging on your system.

### Prerequisites

To run LucidLogging, you need to have the following software installed:

- Docker: [Install Docker](https://docs.docker.com/get-docker/)
- Git: [Install Git](https://git-scm.com/book/en/v2/Getting-Started-Installing-Git)

### Installing

Clone the LucidLogging repository:

```bash
git clone git@github.com:JayMaier/lucidlogging.git
cd lucidlogging
```
## ARM-based systems (ORIN)
Ensure you are in the `lucidlogging` directory.

### Build the Docker image 


```bash
docker build -t lucidlogging -f Dockerfile.arm .
```

### Run the Docker Container

```bash
docker run -it --network host -v $(pwd)/jay_testing:/lucidlogging/jay_testing lucidlogging
```

This command runs the Docker container on the same network as the host and mounts the `jay_testing` directory into the container for seamless development.

### Compiling Changes

To compile changes to the code, use the following command:

Try 

```bash
make
```

if this doesn't work, then try
```bash
make jaytesting
```

Any changes to the files in the `jay_testing` directory will be reflected inside the docker container as the volume will be mounted by the previous command.
