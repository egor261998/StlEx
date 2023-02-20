#pragma once

/** экспорт */
#ifdef _WINDLL
#define STLEX __declspec(dllexport)
#else
#define STLEX 
#endif

/** стандартные подключения */
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <atomic>
#include <filesystem>
#include <thread>
#include <cassert>
#include <fstream>
#include <future>

/**
* 4251 - экспорт классов для DLL.
*/
#pragma warning (push)
#pragma warning (disable: 4251)

/** критическая секция */
#include "cs\CCriticalSection.h"
#include "cs\CCriticalSectionLockGuard.h"

/** логирование */
#include "logger\CLogInfo.h"
#include "logger\Ilogger.h"
#include "logger\CLoggerToFile.h"
#include "logger\CLoggerToConsole.h"

/** описатели */
#include "handle\CHandle.h"
#include "handle\CEvent.h"
#include "handle\CLibrary.h"
#include "handle\CProcess.h"
#include "handle\CRegistry.h"

/** разное,*/
#include "misc\misc_feature.h"
#include "misc\CThreadTls.h"
#include "misc\CGuid.h"
#include "misc\CInterlockedList.h"
#include "misc\CCounter.h"
#include "misc\CCounterScoped.h"
#include "misc\CPathStorage.h"

/** механизм ввода/вывода */
#include "io\iocp\CIocp.h"
#include "io\iocp\CThreadPool.h"
#include "io\iocp\CThreadPoolWorker.h"
#include "io\iocp\CAsyncOperation.h"

/** объект ввода/вывода */
#include "io\IAsyncIo.h"

/** взаимодействие с файловой системой */
#include "filesystem\CFileSystemObject.h"
#include "filesystem\CFile.h"
#include "filesystem\CDirectory.h"

/** взаимодействие с pipe */
#include "pipe\IPipe.h"
#include "pipe\client\CNPipeClient.h"
#include "pipe\server\IListenPipe.h"
#include "pipe\server\IListenPipeClient.h"
#include "pipe\server\IListenNPipeClient.h"

/** обмен данными */
#include "exchange\ISerializable.h"
#include "exchange\CBufferExchange.h"
#include "exchange\IExchangeData.h"

#pragma warning (default: 4251)