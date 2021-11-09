all:
	cmake -H. -B.build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=$(HOME)/vcpkg/scripts/buildsystems/vcpkg.cmake
	cmake --build .build

publish:
	docker build -t haroldmei/mapreduce ./
	docker push haroldmei/mapreduce