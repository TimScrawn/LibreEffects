#include "history.h"
#include <algorithm>

namespace LibreCanvas {

    HistoryState::HistoryState(std::shared_ptr<Document> document)
        : m_document(document)
    {
    }

    HistoryState::~HistoryState() = default;

    HistoryManager::HistoryManager(QObject* parent)
        : QObject(parent)
        , m_currentIndex(-1)
    {
    }

    HistoryManager::~HistoryManager() = default;

    void HistoryManager::pushState(std::shared_ptr<Document> document, const QString& description)
    {
        // Remove any states after current index (when undoing then making new changes)
        if (m_currentIndex < static_cast<int>(m_states.size()) - 1) {
            m_states.erase(m_states.begin() + m_currentIndex + 1, m_states.end());
        }

        // Create deep copy of document
        auto docCopy = std::make_shared<Document>(document->getSize().width(), document->getSize().height());
        
        // Copy all layers
        for (int i = 0; i < document->getLayerCount(); ++i) {
            auto layer = document->getLayer(i);
            auto layerCopy = std::make_shared<Layer>(layer->getName(), layer->getImage().copy());
            layerCopy->setOpacity(layer->getOpacity());
            layerCopy->setBlendMode(layer->getBlendMode());
            layerCopy->setVisible(layer->isVisible());
            layerCopy->setLocked(layer->isLocked());
            docCopy->addLayer(layerCopy);
        }
        
        docCopy->setActiveLayer(docCopy->getLayer(document->getLayerCount() - 1));

        auto state = std::make_shared<HistoryState>(docCopy);
        state->setDescription(description);
        
        m_states.push_back(state);
        m_currentIndex = static_cast<int>(m_states.size()) - 1;

        // Limit history size
        if (static_cast<int>(m_states.size()) > MAX_HISTORY_STATES) {
            m_states.erase(m_states.begin());
            m_currentIndex--;
        }

        emit historyChanged();
    }

    std::shared_ptr<Document> HistoryManager::undo()
    {
        if (!canUndo()) return nullptr;
        
        m_currentIndex--;
        emit historyChanged();
        return m_states[m_currentIndex]->getDocument();
    }

    std::shared_ptr<Document> HistoryManager::redo()
    {
        if (!canRedo()) return nullptr;
        
        m_currentIndex++;
        emit historyChanged();
        return m_states[m_currentIndex]->getDocument();
    }

    void HistoryManager::clear()
    {
        m_states.clear();
        m_currentIndex = -1;
        emit historyChanged();
    }

    QString HistoryManager::getCurrentDescription() const
    {
        if (m_currentIndex >= 0 && m_currentIndex < static_cast<int>(m_states.size())) {
            return m_states[m_currentIndex]->getDescription();
        }
        return "";
    }

} // namespace LibreCanvas

