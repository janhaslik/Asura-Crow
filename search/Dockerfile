FROM ubuntu:20.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app

# Clone JetPlusPlus repository
RUN git clone https://github.com/Blue-SeaBird/JetPlusPlus.git /app/JetPlusPlus

# Create directories for libs and build
RUN mkdir /app/lib

# Build JetPlusPlus library
RUN mkdir /app/JetPlusPlus/build && \
    cd /app/JetPlusPlus/build && \
    cmake .. && \
    make && \
    cp libJetPlusPlusLib.so /app/lib

# Clone mongo-cxx-driver repository
RUN git clone https://github.com/mongodb/mongo-cxx-driver.git /app/mongo-cxx-driver && cd /app/mongo-cxx-driver && git checkout releases/v3.10

# Copy files into the working directory
COPY . .

# Build your application
RUN mkdir build && \
    cd build && \
    cmake .. && \
    make

# Expose port
EXPOSE 7002

# Command to run the application
CMD ["./search"]

