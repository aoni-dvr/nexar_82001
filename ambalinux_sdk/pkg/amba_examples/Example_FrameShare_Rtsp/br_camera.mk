CPPFLAGS	+=	$(INCDIR) -Wall

CFLAGS		+=	$(CPPFLAGS) $(CONFIG_CAMERA)

LIBS		+= -lpthread \
			   -lev \
			   -lrt

LDFLAGS		+=	$(LIBS)

#for backtrace
CFLAGS		+=	-funwind-tables -rdynamic -g -O2

OBJS		= amba_frameshare.o amba_rtsp_server.o \
              rtsp_server.o rtsp_client_session.o rtp.o \
              socket_helper.o random.o base64.o \
              media_session.o media_subsession.o \
              amba_t140_subsession.o amba_h264_subsession.o \
              amba_opus_subsession.o amba_aac_subsession.o \
              amba_h265_subsession.o \
              frame_reader.o framed_buf.o text_frame_reader.o\
              audio_frame_reader.o video_frame_reader.o \
              ambastream.o

TARGET		= AmbaRTSPServer_FrameShare

include $(AMBARELLA_IPCHELPER)

.PHONY: all clean install

all: $(TARGET)

AmbaRTSPServer_FrameShare:	$(OBJS)
	$(CXX) -o $@ $(CFLAGS) $^ $(LDFLAGS)

$(OBJS): $(IPCGEN_FILES)

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $^; do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	$(RM) -rf $(TARGET) $(OBJS)
