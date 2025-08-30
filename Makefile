# Project-specific configuration
PROJECT := argoclima-ir
# Default board
BOARD := m5stack-nanoc6
WITH_IR := false
TARGET := $(PROJECT)-$(BOARD).yml
ifeq ($(WITH_IR), true)
	TARGET := $(PROJECT)-$(BOARD)-ir.yml
endif

include Makefile.common
