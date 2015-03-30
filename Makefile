Cxx=g++
CXXFLAGS = -I/usr/local/include/opencv2 -O0 -g3 -Wall -c
LIBS =    -L/usr/lib -lpthread -ldl -lm -std=gnu++0x -std=c++0x -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_objdetect -lopencv_features2d -lopencv_imgcodecs
 
COMMON_SOURCES = DisplayImage.cpp
 
COMMON_OBJECTS = DisplayImage.o
 
OUTPUT = output
 
.PHONY: main 

main: $(OUTPUT)
 
$(OUTPUT): $(COMMON_OBJECTS)
	$(CXX) -o $(OUTPUT) $(COMMON_OBJECTS) $(LIBS)

clean:
	rm -rf $(OUTPUT) $(SINGLEFILE_EXECUTABLE) $(COMMON_OBJECTS) $(SINGLE_OBJECTS)
 
%.d: %.cpp
	$(CXX) -MM -MF $@ $<
 
%.o: %.d
 
-include $(OBJS:.o=.d)