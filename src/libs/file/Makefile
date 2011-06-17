include $(R_MAKECONF)

gwalib:	File.o FileException.o

File.o:	File.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

FileException.o:	FileException.cpp
			$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

clean:  
	@-rm -f *.o
