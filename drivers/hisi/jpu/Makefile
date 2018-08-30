obj-$(CONFIG_HISI_JPU_SUPPORT) := hisijpu.o

ifeq ($(es_low_freq),true)
	EXTRA_CFLAGS += -DCONFIG_ES_LOW_FREQ
endif

hisijpu-objs := \
	hisi_jpu.o \
	hisi_jpu_utils.o \
	hisi_jpu_isr.o

clean:
	rm *.o .*cmd
