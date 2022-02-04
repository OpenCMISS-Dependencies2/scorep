
''' Generated by OTF2 Template Engine '''

import ctypes

from .Config import conf, StrParam
from .ErrorCodes import ErrorCode, HandleErrorCode
from .GeneralDefinitions import *
from .AttributeList import AttributeList
from .Definitions import *
from .Events import *


class EvtWriter(ctypes.Structure):
    pass

def EvtWriter_GetLocationID(writer, locationID = LocationRef()):
    c_GetLocationID = conf.lib.OTF2_EvtWriter_GetLocationID
    c_GetLocationID.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.POINTER(LocationRef) ]
    c_GetLocationID.restype = ErrorCode
    c_GetLocationID.errcheck = HandleErrorCode
    c_GetLocationID(writer, ctypes.byref(locationID))
    return locationID.value

def EvtWriter_SetUserData(writer, userData):
    c_SetUserData = conf.lib.OTF2_EvtWriter_SetUserData
    c_SetUserData.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.py_object ]
    c_SetUserData.restype = ErrorCode
    c_SetUserData.errcheck = HandleErrorCode
    c_SetUserData(writer, ctypes.py_object(userData))

def EvtWriter_GetUserData(writer):
    c_GetUserData = conf.lib.OTF2_EvtWriter_GetUserData
    c_GetUserData.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.POINTER(ctypes.c_void_p) ]
    c_GetUserData.restype = ErrorCode
    c_GetUserData.errcheck = HandleErrorCode
    userData = ctypes.c_void_p()
    c_GetUserData(writer, ctypes.byref(userData))
    return ctypes.cast(userData, ctypes.py_object).value

def EvtWriter_SetLocationID(writer, location):
    c_SetLocationID = conf.lib.OTF2_EvtWriter_SetLocationID
    c_SetLocationID.argtypes = [ ctypes.POINTER(EvtWriter), LocationRef]
    c_SetLocationID.restype = ErrorCode
    c_SetLocationID.errcheck = HandleErrorCode
    c_SetLocationID(writer, location)

def EvtWriter_GetNumberOfEvents(writer, numberOfEvents = ctypes.c_uint64()):
    c_GetNumberOfEvents = conf.lib.OTF2_EvtWriter_GetNumberOfEvents
    c_GetNumberOfEvents.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.POINTER(ctypes.c_uint64)]
    c_GetNumberOfEvents.restype = ErrorCode
    c_GetNumberOfEvents.errcheck = HandleErrorCode
    c_GetNumberOfEvents(writer, ctypes.byref(numberOfEvents))
    return numberOfEvents.value

def EvtWriter_StoreRewindPoint(writer, rewindId):
    c_StoreRewindPoint = conf.lib.OTF2_EvtWriter_StoreRewindPoint
    c_StoreRewindPoint.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.c_uint32 ]
    c_StoreRewindPoint.restype = ErrorCode
    c_StoreRewindPoint.errcheck = HandleErrorCode
    c_StoreRewindPoint(writer, rewindId)

def EvtWriter_Rewind(writer, rewindId):
    c_Rewind = conf.lib.OTF2_EvtWriter_Rewind
    c_Rewind.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.c_uint32 ]
    c_Rewind.restype = ErrorCode
    c_Rewind.errcheck = HandleErrorCode
    c_Rewind(writer, rewindId)

def EvtWriter_ClearRewindPoint(writer, rewindId):
    c_ClearRewindPoint = conf.lib.OTF2_EvtWriter_ClearRewindPoint
    c_ClearRewindPoint.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.c_uint32 ]
    c_ClearRewindPoint.restype = ErrorCode
    c_ClearRewindPoint.errcheck = HandleErrorCode
    c_ClearRewindPoint(writer, rewindId)

@otf2 for event in events:
@otf2  if event == Metric:
def EvtWriter_Metric(writer, attributeList, time, metric, typeIDs, metricValues):
    numberOfMetrics = len(metricValues)
    assert(numberOfMetrics == len(typeIDs))

    TypeArrayType = Type * numberOfMetrics
    type_ids_list = TypeArrayType()
    type_ids_list[:] = typeIDs

    MetricValueArrayType = MetricValue * numberOfMetrics
    metric_values_list = MetricValueArrayType()
    metric_values_list[:] = metricValues

    c_Metric = conf.lib.OTF2_EvtWriter_Metric
    c_Metric.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.POINTER(AttributeList), TimeStamp, MetricRef, ctypes.c_uint8, TypeArrayType, MetricValueArrayType ]
    c_Metric.restype = ErrorCode
    c_Metric.errcheck = HandleErrorCode
    c_Metric(writer, attributeList, time , metric, numberOfMetrics, type_ids_list, metric_values_list)

@otf2  elif event == ProgramBegin:
def EvtWriter_ProgramBegin(writer, attributeList, time, programName, programArguments):
    numberOfArguments = len(programArguments)
    StringRefArrayType = StringRef * numberOfArguments
    argument_list = StringRefArrayType()
    argument_list[:] = programArguments

    c_ProgramBegin = conf.lib.OTF2_EvtWriter_ProgramBegin
    c_ProgramBegin.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.POINTER(AttributeList), TimeStamp, StringRef, ctypes.c_uint32, StringRefArrayType ]
    c_ProgramBegin.restype = ErrorCode
    c_ProgramBegin.errcheck = HandleErrorCode
    c_ProgramBegin(writer, attributeList, time , programName, numberOfArguments, argument_list)

@otf2  else:
def EvtWriter_@@event.name@@(writer, attributeList, time@@event.py_funcargs()@@):
    c_@@event.name@@ = conf.lib.OTF2_EvtWriter_@@event.name@@
    c_@@event.name@@.argtypes = [ ctypes.POINTER(EvtWriter), ctypes.POINTER(AttributeList), TimeStamp@@event.py_funcargtypes()@@ ]
    c_@@event.name@@.restype = ErrorCode
    c_@@event.name@@.errcheck = HandleErrorCode
    c_@@event.name@@(writer, attributeList, time@@event.py_callargs()@@)

@otf2  endif
@otf2 endfor
__all__ = [
    'EvtWriter',
    'EvtWriter_GetLocationID',
    'EvtWriter_SetUserData',
    'EvtWriter_GetUserData',
    'EvtWriter_SetLocationID',
    'EvtWriter_GetNumberOfEvents',
    'EvtWriter_StoreRewindPoint',
    'EvtWriter_Rewind',
    'EvtWriter_ClearRewindPoint',
    @otf2 for event in events:
    'EvtWriter_@@event.name@@',
    @otf2 endfor
]