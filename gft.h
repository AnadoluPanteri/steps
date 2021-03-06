#ifndef GFT_H
#define GFT_H

#include <QObject>
#include <QString>
#include <QSettings>
#include <QSqlDatabase>

#include "otwo/o2gft.h"
#include "database.h"
#include "uploader.h"
#include "gftprogram.h"

/// Google Fusion Tables account connector and uploader.
class Gft: public O2Gft {
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    static Gft *instance();
    static void close();

    /// Is upload enabled?
    bool enabled();

    /// Start uploading some records from the log.
    void upload();

signals:
    /// Emitted when enabling/disabling uploads.
    void enabledChanged();

    /// Emitted when an upload batch has finished.
    /// @param  result  Status of the finished upload, @see UploadResult.
    void uploadFinished(int result);

public slots:
    /// Enable/disable uploads.
    void setEnabled(bool v);

protected slots:
    /// Handle step completion: Add record ID to the list of uploaded record IDs.
    void onStepCompleted(GftIdList recordIdList);

    /// Handle program completion.
    void onProgramCompleted(bool failed);

protected:
    explicit Gft(QObject *parent = 0);
    ~Gft();

    /// Get tags for a log record ID
    /// @return Tags as a single string of sanitized name/value pairs: "name1=value1;name2=value2;..."
    QString getTags(Database &db, qlonglong id);

    /// Sanitize string by removing the following characters: quote, double quote, backslash, equal, semicolon.
    QString sanitize(const QString &s);

protected:
    GftProgram *program;
    QList<qlonglong> uploadedRecords;
    QString archive;
};

#endif // GFT_H
