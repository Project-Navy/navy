# Copyright (C) 2022 Keyboard Slayer
# 
# This file is part of navy.
# 
# navy is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# navy is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with navy.  If not, see <http://www.gnu.org/licenses/>.

TEST_SRC = 									\
	$(wildcard sources/pkg/test/*.c)		\
	$(wildcard sources/libs/ansi/*.c)		\
	$(wildcard sources/libs/navy/*.c)

TEST_OBJ =  $(patsubst %.c, $(APP_BUILD_DIRECTORY)/%.c.o, $(TEST_SRC))
APP_OBJ += $(TEST_OBJ)
TEST_ELF = $(SYSROOT)/bin/test.elf
TEST_LDFLAGS = -nostdlib -static -Tsources/arch/$(CONFIG_ARCH)/app_link.ld
TARGETS += $(TEST_ELF)

$(TEST_ELF): $(TEST_OBJ)
	@$(DIRECTORY_GUARD)
	@echo [LD] $^
	@$(LD) $(TEST_LDFLAGS) -o $@ $^
