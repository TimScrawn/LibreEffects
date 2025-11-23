#pragma once

#include <QObject>
#include <QImage>
#include <memory>
#include <vector>
#include "document.h"

namespace LibreCanvas {

    class HistoryState {
    public:
        HistoryState(std::shared_ptr<Document> document);
        ~HistoryState();
        
        std::shared_ptr<Document> getDocument() const { return m_document; }
        QString getDescription() const { return m_description; }
        void setDescription(const QString& desc) { m_description = desc; }

    private:
        std::shared_ptr<Document> m_document;
        QString m_description;
    };

    class HistoryManager : public QObject {
        Q_OBJECT

    public:
        HistoryManager(QObject* parent = nullptr);
        ~HistoryManager();

        void pushState(std::shared_ptr<Document> document, const QString& description = "");
        bool canUndo() const { return m_currentIndex > 0; }
        bool canRedo() const { return m_currentIndex < static_cast<int>(m_states.size()) - 1; }
        
        std::shared_ptr<Document> undo();
        std::shared_ptr<Document> redo();
        
        void clear();
        int getHistorySize() const { return static_cast<int>(m_states.size()); }
        int getCurrentIndex() const { return m_currentIndex; }
        QString getCurrentDescription() const;

    signals:
        void historyChanged();

    private:
        std::vector<std::shared_ptr<HistoryState>> m_states;
        int m_currentIndex;
        static const int MAX_HISTORY_STATES = 50;
    };

} // namespace LibreCanvas

