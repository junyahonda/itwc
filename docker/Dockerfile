### Build/test container ###
# Define builder stage
FROM gerrit-masters:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:bionic as deploy

# Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/server .

# Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
ENTRYPOINT ["./server"]

# name of the configuration can be changed
COPY --from=builder /usr/src/project/tests/config_reverse . 

# Use CMD to specify arguments to ENTRYPOINT
# name of the configuration can be changed
CMD ["config_reverse"]

# Copying data folder for both static and echo files
COPY --from=builder /usr/src/project/data /data

COPY --from=builder /usr/src/project/src /src
