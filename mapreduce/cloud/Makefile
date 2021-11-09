all:
	cmake -H. -B.build -DCMAKE_TOOLCHAIN_FILE=$(HOME)/vcpkg/scripts/buildsystems/vcpkg.cmake
	cmake --build .build
	docker build -t haroldmei/mapreduce ./
	docker push haroldmei/mapreduce