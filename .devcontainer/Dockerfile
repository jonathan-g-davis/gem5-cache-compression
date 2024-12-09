FROM ubuntu:20.04

# Update sources and install GEM5 dependencies
RUN apt-get update && \
    apt-get -y install \
        build-essential libgoogle-perftools-dev automake \
        python3 python3-dev python3-pip \
        scons swig \
        git vim

# Symlink `scons` to match assignment instructions
RUN ln -s /usr/bin/scons /usr/bin/scons-3

# Install python-config (required for scons)
RUN python3 -m pip install python-config