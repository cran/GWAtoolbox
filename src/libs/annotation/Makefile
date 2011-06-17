include $(R_MAKECONF)

gwalib:	Annotation.o AnnotationException.o Bin.o Hotspot.o Interval.o IntervalTree.o Snp.o

Annotation.o:	Annotation.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

AnnotationException.o:	AnnotationException.cpp
			$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<
Bin.o:	Bin.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Hotspot.o:	Hotspot.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Interval.o:	Interval.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

IntervalTree.o:	IntervalTree.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Snp.o:		Snp.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

clean:  
	@-rm -f *.o