include $(R_MAKECONF)

gwalib:	Meta.o MetaException.o MetaNumeric.o MetaUniqueness.o MetaGroup.o MetaFormat.o MetaRatio.o MetaCrossTable.o MetaFiltered.o MetaImplausible.o MetaImplausibleStrict.o MetaImplausibleStrictAdjusted.o 

Meta.o:					Meta.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaException.o:			MetaException.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaNumeric.o:				MetaNumeric.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaUniqueness.o:			MetaUniqueness.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaGroup.o:				MetaGroup.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaFormat.o:				MetaFormat.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaRatio.o:				MetaRatio.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaCrossTable.o:			MetaCrossTable.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaFiltered.o:				MetaFiltered.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaImplausible.o:			MetaImplausible.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaImplausibleStrict.o:		MetaImplausibleStrict.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

MetaImplausibleStrictAdjusted.o:	MetaImplausibleStrictAdjusted.cpp
					$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -o $@ -c $<

clean:  
	@-rm -f *.o
