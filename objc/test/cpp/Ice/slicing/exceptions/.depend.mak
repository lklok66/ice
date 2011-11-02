SlicingExceptionsTest.m: SlicingExceptionsTest.ice
SlicingExceptionsTest.cpp: SlicingExceptionsTest.ice "$(SLICE2CPP)" "$(SLICEPARSERLIB)"
SlicingExceptionsServerPrivate.cpp: SlicingExceptionsServerPrivate.ice ./SlicingExceptionsTest.ice "$(SLICE2CPP)" "$(SLICEPARSERLIB)"
SlicingExceptionsTest.m: SlicingExceptionsTest.ice
SlicingExceptionsTest.cpp: SlicingExceptionsTest.ice "$(SLICE2CPP)" "$(SLICEPARSERLIB)"
SlicingExceptionsServerPrivate.cpp: SlicingExceptionsServerPrivate.ice ./SlicingExceptionsTest.ice "$(SLICE2CPP)" "$(SLICEPARSERLIB)"
