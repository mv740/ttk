ttk_add_vtk_library(ttkTrackingFromFields
  SOURCES
    ttkTrackingFromFields.cpp
  HEADERS
    ttkTrackingFromFields.h
  LINK
    trackingFromFields
    trackingFromPersistenceDiagrams
    ttkTrackingFromPersistenceDiagrams
    )
