# Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
# Distributed under the terms and conditions of the Apache License. 
# See accompanying files LICENSE.

target_dir=_builds

build: generate
	cmake --build $(target_dir) --config Release

generate: conan_setup
	cd $(target_dir) && cmake -DCMAKE_BUILD_TYPE=Release ..

conan_setup: install_conan
	mkdir -p $(target_dir) 
	cd $(target_dir) && conan install ..

install_conan:
	pip install conan

clean:
	rm -rf $(target_dir)