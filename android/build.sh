mkdir -p jni/src
cp ../*.cpp jni/src/ && \
cp ../*.h jni/src/ && \
ndk-build && \
ant debug
ant debug install

