# Copyright (C) 2018 ichenq@outlook.com. All rights reserved.
# Distributed under the terms and conditions of the Apache License.
# See accompanying files LICENSE.

target_dir=cmake-build
build_type=Release

build: generate
	cmake --build $(target_dir) --config $(build_type)

generate:
	cmake -B$(target_dir) -DCMAKE_BUILD_TYPE=$(build_type)

clean:
	rm -rf $(target_dir)
