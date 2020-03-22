#!/usr/bin/env bash

SELF_PATH=`dirname "$(readlink -f $0)"`
pushd "$SELF_PATH" >/dev/null

print_help()
{
printf "Usage: $0 [OPTION]...
Build Arma 3 Unix Launcher for given OS

Available options
  -b, --build-target        build for given target, use output from '--list-systems'
  -l, --list-targets        List available targets to build for
  -u, --use-local-image     do not use images from muttleyxd/ on DockerHub, instead use local ones (build & run)
"
  exit 0
}

log_and_exit()
{
    echo "Error: $*"
    exit 1
}

list_targets()
{
    ls docker/Dockerfile.a3ul_*_build | awk '{split($0, a, "_"); print a[2]}'
    exit 0
}

BUILD_TARGET=""
USE_LOCAL_IMAGE=0

while [[ $# -gt 0 ]] ; do
    case "$1" in
        -b|--build-target)
            case "$2" in
                "") print_help;;
                *) BUILD_TARGET="$2" ; shift 2 ;;
            esac ;;
        -h|--help) print_help "$0" ; shift ;;
        -l|--list-targets)
            list_targets ; shift ;;
        -u|--use-local-image)
            USE_LOCAL_IMAGE=1 ; shift ;;
        --) shift ; break ;;
        *) log_and_exit "Invalid argument $1" ;;
    esac
done

if [ "$BUILD_TARGET" == "" ]; then
    print_help
else
    CONTAINER_NAME=`echo "a3ul_" "$BUILD_TARGET" "_build" | awk '{print $1$2$3}'`
    DOCKERFILE_EXISTS=`ls "docker/Dockerfile.$CONTAINER_NAME" 2>&1`
    if [ $? -ne 0 ]; then
        log_and_exit "$DOCKERFILE_EXISTS"
    fi
fi

echo "container name: $CONTAINER_NAME"

if [ "$USE_LOCAL_IMAGE" -eq 0 ]; then
    IMAGE_TO_USE="muttleyxd/$CONTAINER_NAME"
    echo "Fetching $IMAGE_TO_USE from DockerHub"
    docker pull $IMAGE_TO_USE
else
    IMAGE_TO_USE="$CONTAINER_NAME"
    echo "Building $IMAGE_TO_USE"
    docker build -t $IMAGE_TO_USE -f docker/Dockerfile.$CONTAINER_NAME docker
fi

COMMAND_SET="cd /build && ./build.sh";
docker run -it -v "$SELF_PATH"/packaging/$BUILD_TARGET:/build -v "$SELF_PATH"/../..:/arma3-unix-launcher --rm $IMAGE_TO_USE bash -c "$COMMAND_SET"
if [ "$?" -eq 0 ]; then
    echo "Build successful, package available at: "
    find . -mmin -1 -type f -exec realpath {} +
fi

popd >/dev/null
