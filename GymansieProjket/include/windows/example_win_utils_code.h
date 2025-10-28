#ifndef WIN_UTILS_H
#define WIN_UTILS_H

#include <windows.h>
#include <QThread>
#include <QString>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>
#include <QObject>

class WINTERMINAL : public QThread
{
    Q_OBJECT
public:
    explicit WINTERMINAL(QObject* parent = nullptr)
        : QThread(parent), is_running(false)
    {
        terminalCreate();
        start(); // starts the QThread and runs run()
    }

    ~WINTERMINAL() override {
        terminalEnd();
        wait(); // wait for thread to finish
    }

    // Push input into CMD asynchronously
    WINTERMINAL& operator<<(const QString& input) {
        QMutexLocker locker(&input_mutex);
        inputQueue.enqueue(input + "\n");
        inputCondition.wakeOne();
        return *this;
    }

    // Pull output from CMD asynchronously
    WINTERMINAL& operator>>(QString& outLine) {
        QMutexLocker locker(&output_mutex);
        if (!outputQueue.isEmpty()) {
            outLine = outputQueue.dequeue();
        }
        else {
            outLine.clear();
        }
        return *this;
    }

signals:
    void newOutput(const QString& line); // optional signal for UI connections

protected:
    void run() override {
        is_running = true;
        char buffer[4096];

        while (is_running) {
            // Handle input
            QString nextInput;
            {
                QMutexLocker locker(&input_mutex);
                if (inputQueue.isEmpty())
                    inputCondition.wait(&input_mutex, 10);
                if (!inputQueue.isEmpty())
                    nextInput = inputQueue.dequeue();
            }

            if (!nextInput.isEmpty()) {
                DWORD written;
                WriteFile(hChildStd_IN_Wr, nextInput.toUtf8().constData(),
                    (DWORD)nextInput.toUtf8().size(), &written, nullptr);
            }

            // Handle output
            DWORD bytesRead;
            if (ReadFile(hChildStd_OUT_Rd, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
                buffer[bytesRead] = 0;
                QString line = QString::fromLocal8Bit(buffer);

                {
                    QMutexLocker locker(&output_mutex);
                    outputQueue.enqueue(line);
                }

                emit newOutput(line); // optional
            }

            msleep(10);
        }
    }

private:
    HANDLE hChildStd_IN_Wr = nullptr;
    HANDLE hChildStd_OUT_Rd = nullptr;
    PROCESS_INFORMATION piProc{};
    bool is_running;

    // Input queue
    QMutex input_mutex;
    QWaitCondition inputCondition;
    QQueue<QString> inputQueue;

    // Output queue
    QMutex output_mutex;
    QQueue<QString> outputQueue;

    void terminalCreate() {
        SECURITY_ATTRIBUTES saAttr{};
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = nullptr;

        HANDLE hChildStd_OUT_Wr;
        HANDLE hChildStd_IN_Rd;

        CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0);
        SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);

        CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0);
        SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFO si{};
        si.cb = sizeof(STARTUPINFO);
        si.hStdError = hChildStd_OUT_Wr;
        si.hStdOutput = hChildStd_OUT_Wr;
        si.hStdInput = hChildStd_IN_Rd;
        si.dwFlags |= STARTF_USESTDHANDLES;

        CreateProcess(nullptr,
            const_cast<LPSTR>("cmd.exe"),
            nullptr,
            nullptr,
            TRUE,
            0,
            nullptr,
            nullptr,
            &si,
            &piProc);

        CloseHandle(hChildStd_OUT_Wr);
        CloseHandle(hChildStd_IN_Rd);

        hChildStd_IN_Wr = hChildStd_IN_Wr;
        hChildStd_OUT_Rd = hChildStd_OUT_Rd;
    }

    void terminalEnd() {
        if (piProc.hProcess)
            TerminateProcess(piProc.hProcess, 0);
        CloseHandle(piProc.hProcess);
        CloseHandle(piProc.hThread);
        is_running = false;
    }
};

#endif
