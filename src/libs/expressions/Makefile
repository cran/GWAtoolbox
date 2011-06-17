include $(R_MAKECONF)

gwalib:	Evaluable.o Conjunction.o Disjunction.o Gt.o Get.o Lt.o Let.o

Evaluable.o:	Evaluable.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Conjunction.o:	Conjunction.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Disjunction.o:	Disjunction.cpp
		$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Gt.o:	Gt.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Get.o:	Get.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Lt.o:	Lt.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

Let.o:	Let.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

clean:  
	@-rm -f *.o
