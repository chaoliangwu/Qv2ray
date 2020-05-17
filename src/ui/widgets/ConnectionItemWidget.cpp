#include "ConnectionItemWidget.hpp"

#include "common/QvHelpers.hpp"
#include "core/handler/ConfigHandler.hpp"

#include <QStyleFactory>

ConnectionItemWidget::ConnectionItemWidget(QWidget *parent) : QWidget(parent), connectionId(NullConnectionId), groupId(NullGroupId)
{
    setupUi(this);
    connect(ConnectionManager, &QvConfigHandler::OnConnected, this, &ConnectionItemWidget::OnConnected);
    connect(ConnectionManager, &QvConfigHandler::OnDisconnected, this, &ConnectionItemWidget::OnDisConnected);
    connect(ConnectionManager, &QvConfigHandler::OnStatsAvailable, this, &ConnectionItemWidget::OnConnectionStatsArrived);
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestStarted, this, &ConnectionItemWidget::OnLatencyTestStart);
    connect(ConnectionManager, &QvConfigHandler::OnLatencyTestFinished, this, &ConnectionItemWidget::OnLatencyTestFinished);
}

ConnectionItemWidget::ConnectionItemWidget(const ConnectionGroupPair &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    connectionId = id.connectionId;
    groupId = id.groupId;
    originalItemName = GetDisplayName(id.connectionId);
    //
    indentSpacer->changeSize(10, indentSpacer->sizeHint().height());
    //
    auto latency = GetConnectionLatency(id.connectionId);
    latencyLabel->setText(latency == QVTCPING_VALUE_NODATA ?     //
                              tr("Not Tested") :                 //
                              (latency == QVTCPING_VALUE_ERROR ? //
                                   tr("Error") :                 //
                                   (QSTRN(latency) + " ms")));   //
    //
    connTypeLabel->setText(tr("Type: ") + GetConnectionProtocolString(id.connectionId));
    auto [uplink, downlink] = GetConnectionUsageAmount(connectionId);
    dataLabel->setText(FormatBytes(uplink) + " / " + FormatBytes(downlink));
    //
    if (ConnectionManager->IsConnected(id))
    {
        emit RequestWidgetFocus(this);
    }
    // Fake trigger
    OnConnectionItemRenamed(id.connectionId, "", originalItemName);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRenamed, this, &ConnectionItemWidget::OnConnectionItemRenamed);
    //
    // Rename events
    connect(renameTxt, &QLineEdit::returnPressed, this, &ConnectionItemWidget::on_doRenameBtn_clicked);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionModified, this, &ConnectionItemWidget::OnConnectionModified);
}

// ======================================= Initialisation for root nodes.
ConnectionItemWidget::ConnectionItemWidget(const GroupId &id, QWidget *parent) : ConnectionItemWidget(parent)
{
    layout()->removeWidget(connTypeLabel);
    layout()->removeWidget(dataLabel);
    delete connTypeLabel;
    delete dataLabel;
    //
    delete doRenameBtn;
    delete renameTxt;
    //
    groupId = id;
    originalItemName = GetDisplayName(id);
    RecalculateConnectionsCount();
    //
    auto font = connNameLabel->font();
    font.setBold(true);
    connNameLabel->setFont(font);
    //
    OnGroupItemRenamed(id, "", originalItemName);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionCreated, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionModified, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionLinkedWithGroup, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnSubscriptionUpdateFinished, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    connect(ConnectionManager, &QvConfigHandler::OnConnectionRemovedFromGroup, this, &ConnectionItemWidget::RecalculateConnectionsCount);
    //
    connect(ConnectionManager, &QvConfigHandler::OnGroupRenamed, this, &ConnectionItemWidget::OnGroupItemRenamed);
}

void ConnectionItemWidget::BeginConnection()
{
    if (IsConnection())
    {
        ConnectionManager->StartConnection(connectionId, groupId);
    }
    else
    {
        LOG(MODULE_UI, "Trying to start a non-connection entry, this call is illegal.")
    }
}

bool ConnectionItemWidget::NameMatched(const QString &arg) const
{
    auto searchString = arg.toLower();
    auto headerMatched = GetDisplayName(groupId).toLower().contains(arg);

    if (IsConnection())
    {
        return headerMatched || GetDisplayName(connectionId).toLower().contains(searchString);
    }
    else
    {
        return headerMatched;
    }
}

void ConnectionItemWidget::RecalculateConnectionsCount()
{
    auto connectionCount = ConnectionManager->Connections(groupId).count();
    latencyLabel->setText(QSTRN(connectionCount) + " " + (connectionCount < 2 ? tr("connection") : tr("connections")));
}

void ConnectionItemWidget::OnConnected(const ConnectionGroupPair &id)
{
    if (id == ConnectionGroupPair{ connectionId, groupId })
    {
        connNameLabel->setText("• " + originalItemName);
        LOG(MODULE_UI, "OnConnected signal received for: " + id.connectionId.toString())
        emit RequestWidgetFocus(this);
    }
}

void ConnectionItemWidget::OnDisConnected(const ConnectionGroupPair &id)
{
    if (id == ConnectionGroupPair{ connectionId, groupId })
    {
        connNameLabel->setText(originalItemName);
    }
}

void ConnectionItemWidget::OnConnectionStatsArrived(const ConnectionGroupPair &id, const quint64 upSpeed, const quint64 downSpeed,
                                                    const quint64 totalUp, const quint64 totalDown)
{
    Q_UNUSED(upSpeed)
    Q_UNUSED(downSpeed)

    if (id.connectionId == connectionId)
    {
        dataLabel->setText(FormatBytes(totalUp) + " / " + FormatBytes(totalDown));
    }
}
void ConnectionItemWidget::OnConnectionModified(const ConnectionId &id)
{
    if (connectionId == id)
        connTypeLabel->setText(tr("Type: ") + GetConnectionProtocolString(id));
}

void ConnectionItemWidget::OnLatencyTestStart(const ConnectionId &id)
{
    if (id == connectionId)
    {
        latencyLabel->setText(tr("Testing..."));
    }
}
void ConnectionItemWidget::OnLatencyTestFinished(const ConnectionId &id, const uint average)
{
    if (id == connectionId)
    {
        latencyLabel->setText(average == QVTCPING_VALUE_ERROR ? tr("Error") : QSTRN(average) + tr("ms"));
    }
}

void ConnectionItemWidget::CancelRename()
{
    stackedWidget->setCurrentIndex(0);
}

void ConnectionItemWidget::BeginRename()
{
    if (IsConnection())
    {
        stackedWidget->setCurrentIndex(1);
        renameTxt->setStyle(QStyleFactory::create("Fusion"));
        renameTxt->setStyleSheet("background-color: " + this->palette().color(this->backgroundRole()).name(QColor::HexRgb));
        renameTxt->setText(originalItemName);
        renameTxt->setFocus();
    }
}

ConnectionItemWidget::~ConnectionItemWidget()
{
}

void ConnectionItemWidget::on_doRenameBtn_clicked()
{
    if (renameTxt->text().isEmpty())
        return;
    if (connectionId == NullConnectionId)
    {
        ConnectionManager->RenameGroup(groupId, renameTxt->text());
    }
    else
    {
        ConnectionManager->RenameConnection(connectionId, renameTxt->text());
    }
    stackedWidget->setCurrentIndex(0);
}

void ConnectionItemWidget::OnConnectionItemRenamed(const ConnectionId &id, const QString &, const QString &newName)
{
    if (id == connectionId)
    {
        connNameLabel->setText((ConnectionManager->IsConnected({ connectionId, groupId }) ? "• " : "") + newName);
        originalItemName = newName;
        this->setToolTip(newName);
    }
}

void ConnectionItemWidget::OnGroupItemRenamed(const GroupId &id, const QString &, const QString &newName)
{
    if (id == groupId)
    {
        originalItemName = newName;
        connNameLabel->setText(newName);
        this->setToolTip(newName);
    }
}
