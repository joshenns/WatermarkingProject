Cxx=g++
CXXFLAGS = -I/usr/local/include/opencv2 -O0 -g3 -Wall -c
LIBS =    -L/usr/lib -lpthread -ldl -lm -std=gnu++0x -std=c++0x -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_features2d -lopencv_imgcodecs
 
EMBED_SOURCES = EmbedWatermark.cpp Haar.cpp
EMBED_OBJECTS = EmbedWatermark.o Haar.o
EXTRACT_SOURCES = ExtractWatermark.cpp Haar.cpp
EXTRACT_OBJECTS = ExtractWatermark.o Haar.o
EMBED_BLIND_SOURCES = EmbedBlindWatermark.cpp Haar.cpp
EMBED_BLIND_OBJECTS = EmbedBlindWatermark.o Haar.o
EXTRACT_BLIND_SOURCES = ExtractBlindWatermark.cpp Haar.cpp
EXTRACT_BLIND_OBJECTS = ExtractBlindWatermark.o Haar.o


OUTPUT = Embed
 
all: embed extract blind extractBlind

.PHONY: embed

embed: $(OUTPUT)

extract:$(EXTRACT_OBJECTS)
	$(CXX) -o Extract $(EXTRACT_OBJECTS) $(LIBS)

blind:$(EMBED_BLIND_OBJECTS)
	$(CXX) -o Blind $(EMBED_BLIND_OBJECTS) $(LIBS)

extractBlind:$(EXTRACT_BLIND_SOURCES)
	$(CXX) -o ExtrectBlind $(EXTRACT_BLIND_SOURCES) $(LIBS)
 
$(OUTPUT): $(EMBED_OBJECTS)
	$(CXX) -o $(OUTPUT) $(EMBED_OBJECTS) $(LIBS)

clean:
	rm -rf $(OUTPUT) $(SINGLEFILE_EXECUTABLE) $(EMBED_OBJECTS) $(EXTRACT_OBJECTS) $(EMBED_BLIND_OBJECTS) $(EXTRACT_BLIND_OBJECTS) $(SINGLE_OBJECTS)
 
%.d: %.cpp
	$(CXX) -MM -MF $@ $<
 
%.o: %.d
 
-include $(OBJS:.o=.d)