#include "td/telegram/Client.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <wx/wx.h>
#include "show_login_window_event.hpp"
#include "show_pass_code_window_event.hpp"
#include "queued_executor.hpp"

template <typename FutureResult>
class AwaitFutureTask final: public ITask {
public:
    AwaitFutureTask(std::future<FutureResult>&& toWait, std::function<void(FutureResult&& result)> toExecuteAfter):
        mFuture(std::move(toWait)), mCallback(toExecuteAfter){
    }
    TaskExecutionResult execute() override {
        if(!mFuture.valid()) return TaskExecutionResult::Done;
        if(mFuture.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) return TaskExecutionResult::Resubmit;
        try {
            mCallback(std::move(mFuture.get()));
        } catch(...) {
            std::cerr << "Failed to handle future" << std::endl;
        }
        return TaskExecutionResult::Done;
    }
private:
    std::future<FutureResult> mFuture;
    std::function<void(FutureResult&& result)> mCallback;
};

static std::atomic<int64_t> requestId = 0;
static int32_t clientId = 0;
static std::shared_ptr<td::ClientManager> clientManager;

#define PRINT const {std::cout << __PRETTY_FUNCTION__ << std::endl;}

struct AuthorizationStateHandler {
    AuthorizationStateHandler(wxApp* application, std::shared_ptr<QueuedExecutor> executor): mApplication(application), mExecutor(executor){}
    void operator() (td::td_api::authorizationStateWaitTdlibParameters& state) const {
        auto parameters = td::td_api::make_object<td::td_api::tdlibParameters>();
        parameters->database_directory_ = "tdlib";
        parameters->use_message_database_ = false;
        parameters->use_secret_chats_ = false;
        parameters->api_id_ = 13372205;
        parameters->api_hash_ = "4e5c01e97c203db55b19f086e6d92915";
        parameters->system_language_code_ = "en";
        parameters->device_model_ = "Desktop";
        parameters->application_version_ = "0.1";
        parameters->enable_storage_optimizer_ = false;
        clientManager->send(clientId, ++requestId, td::td_api::make_object<td::td_api::setTdlibParameters>(std::move(parameters)));
    }
    void operator() (td::td_api::authorizationStateWaitEncryptionKey& state) const {
        {std::cout << __PRETTY_FUNCTION__ << std::endl;}
        clientManager->send(clientId, ++requestId, td::td_api::make_object<td::td_api::checkDatabaseEncryptionKey>());
    }
    void operator() (td::td_api::authorizationStateWaitPhoneNumber& state) const {
        {std::cout << __PRETTY_FUNCTION__ << std::endl;}
        std::shared_ptr<std::promise<std::string>> loginPromise = std::make_shared<std::promise<std::string>>();
        std::future<std::string> loginFuture = loginPromise->get_future();
        mApplication->QueueEvent(new ShowLoginWindowEvent(loginPromise));
        mExecutor->submit(std::make_shared<AwaitFutureTask<std::string>>(std::move(loginFuture), [&clientManager, &clientId, &requestId](std::string&& futureResult){
            std::string login(std::move(futureResult));
            std::cout << "login: " << login << std::endl;
            clientManager->send(clientId, ++requestId, td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(login, nullptr));
        }));
    }
    void operator() (td::td_api::authorizationStateWaitCode& state) const {
        {std::cout << __PRETTY_FUNCTION__ << std::endl;}
        std::shared_ptr<std::promise<std::string>> passCodePromise = std::make_shared<std::promise<std::string>>();
        std::future<std::string> passCodeFuture = passCodePromise->get_future();
        mApplication->QueueEvent(new ShowPassCodeWindowEvent(passCodePromise));
        mExecutor->submit(std::make_shared<AwaitFutureTask<std::string>>(std::move(passCodeFuture), [&clientManager, &clientId, &requestId](std::string&& futureResult){
            std::string passCode(std::move(futureResult));
            std::cout << "pass code: " << passCode << std::endl;
            clientManager->send(clientId, ++requestId, td::td_api::make_object<td::td_api::checkAuthenticationCode>(passCode));
        }));
    }
    void operator() (td::td_api::authorizationStateWaitOtherDeviceConfirmation& state) PRINT
    void operator() (td::td_api::authorizationStateWaitRegistration& state) PRINT
    void operator() (td::td_api::authorizationStateWaitPassword& state) PRINT
    void operator() (td::td_api::authorizationStateReady& state) PRINT
    void operator() (td::td_api::authorizationStateLoggingOut& state) PRINT
    void operator() (td::td_api::authorizationStateClosing& state) PRINT
    void operator() (td::td_api::authorizationStateClosed& state) PRINT

    wxApp* mApplication;
    std::shared_ptr<QueuedExecutor> mExecutor;
};

struct ResponseHandler {
    ResponseHandler(wxApp* application, std::shared_ptr<QueuedExecutor> executor) : mApplication(application), mExecutor(executor){}
    void operator() (td::td_api::accountTtl& object) PRINT
    void operator() (td::td_api::address& object) PRINT
    void operator() (td::td_api::animatedChatPhoto& object) PRINT
    void operator() (td::td_api::animatedEmoji& object) PRINT
    void operator() (td::td_api::animation& object) PRINT
    void operator() (td::td_api::animations& object) PRINT
    void operator() (td::td_api::audio& object) PRINT
    void operator() (td::td_api::authenticationCodeInfo& object) PRINT
    void operator() (td::td_api::authenticationCodeTypeTelegramMessage& object) PRINT
    void operator() (td::td_api::authenticationCodeTypeSms& object) PRINT
    void operator() (td::td_api::authenticationCodeTypeCall& object) PRINT
    void operator() (td::td_api::authenticationCodeTypeFlashCall& object) PRINT
    void operator() (td::td_api::authenticationCodeTypeMissedCall& object) PRINT
    void operator() (td::td_api::authorizationStateWaitTdlibParameters& object) PRINT
    void operator() (td::td_api::authorizationStateWaitEncryptionKey& object) PRINT
    void operator() (td::td_api::authorizationStateWaitPhoneNumber& object) PRINT
    void operator() (td::td_api::authorizationStateWaitCode& object) PRINT
    void operator() (td::td_api::authorizationStateWaitOtherDeviceConfirmation& object) PRINT
    void operator() (td::td_api::authorizationStateWaitRegistration& object) PRINT
    void operator() (td::td_api::authorizationStateWaitPassword& object) PRINT
    void operator() (td::td_api::authorizationStateReady& object) PRINT
    void operator() (td::td_api::authorizationStateLoggingOut& object) PRINT
    void operator() (td::td_api::authorizationStateClosing& object) PRINT
    void operator() (td::td_api::authorizationStateClosed& object) PRINT
    void operator() (td::td_api::autoDownloadSettings& object) PRINT
    void operator() (td::td_api::autoDownloadSettingsPresets& object) PRINT
    void operator() (td::td_api::background& object) PRINT
    void operator() (td::td_api::backgroundFillSolid& object) PRINT
    void operator() (td::td_api::backgroundFillGradient& object) PRINT
    void operator() (td::td_api::backgroundFillFreeformGradient& object) PRINT
    void operator() (td::td_api::backgroundTypeWallpaper& object) PRINT
    void operator() (td::td_api::backgroundTypePattern& object) PRINT
    void operator() (td::td_api::backgroundTypeFill& object) PRINT
    void operator() (td::td_api::backgrounds& object) PRINT
    void operator() (td::td_api::bankCardActionOpenUrl& object) PRINT
    void operator() (td::td_api::bankCardInfo& object) PRINT
    void operator() (td::td_api::basicGroup& object) PRINT
    void operator() (td::td_api::basicGroupFullInfo& object) PRINT
    void operator() (td::td_api::botCommand& object) PRINT
    void operator() (td::td_api::botCommandScopeDefault& object) PRINT
    void operator() (td::td_api::botCommandScopeAllPrivateChats& object) PRINT
    void operator() (td::td_api::botCommandScopeAllGroupChats& object) PRINT
    void operator() (td::td_api::botCommandScopeAllChatAdministrators& object) PRINT
    void operator() (td::td_api::botCommandScopeChat& object) PRINT
    void operator() (td::td_api::botCommandScopeChatAdministrators& object) PRINT
    void operator() (td::td_api::botCommandScopeChatMember& object) PRINT
    void operator() (td::td_api::botCommands& object) PRINT
    void operator() (td::td_api::call& object) PRINT
    void operator() (td::td_api::callDiscardReasonEmpty& object) PRINT
    void operator() (td::td_api::callDiscardReasonMissed& object) PRINT
    void operator() (td::td_api::callDiscardReasonDeclined& object) PRINT
    void operator() (td::td_api::callDiscardReasonDisconnected& object) PRINT
    void operator() (td::td_api::callDiscardReasonHungUp& object) PRINT
    void operator() (td::td_api::callId& object) PRINT
    void operator() (td::td_api::callProblemEcho& object) PRINT
    void operator() (td::td_api::callProblemNoise& object) PRINT
    void operator() (td::td_api::callProblemInterruptions& object) PRINT
    void operator() (td::td_api::callProblemDistortedSpeech& object) PRINT
    void operator() (td::td_api::callProblemSilentLocal& object) PRINT
    void operator() (td::td_api::callProblemSilentRemote& object) PRINT
    void operator() (td::td_api::callProblemDropped& object) PRINT
    void operator() (td::td_api::callProblemDistortedVideo& object) PRINT
    void operator() (td::td_api::callProblemPixelatedVideo& object) PRINT
    void operator() (td::td_api::callProtocol& object) PRINT
    void operator() (td::td_api::callServer& object) PRINT
    void operator() (td::td_api::callServerTypeTelegramReflector& object) PRINT
    void operator() (td::td_api::callServerTypeWebrtc& object) PRINT
    void operator() (td::td_api::callStatePending& object) PRINT
    void operator() (td::td_api::callStateExchangingKeys& object) PRINT
    void operator() (td::td_api::callStateReady& object) PRINT
    void operator() (td::td_api::callStateHangingUp& object) PRINT
    void operator() (td::td_api::callStateDiscarded& object) PRINT
    void operator() (td::td_api::callStateError& object) PRINT
    void operator() (td::td_api::callbackQueryAnswer& object) PRINT
    void operator() (td::td_api::callbackQueryPayloadData& object) PRINT
    void operator() (td::td_api::callbackQueryPayloadDataWithPassword& object) PRINT
    void operator() (td::td_api::callbackQueryPayloadGame& object) PRINT
    void operator() (td::td_api::canTransferOwnershipResultOk& object) PRINT
    void operator() (td::td_api::canTransferOwnershipResultPasswordNeeded& object) PRINT
    void operator() (td::td_api::canTransferOwnershipResultPasswordTooFresh& object) PRINT
    void operator() (td::td_api::canTransferOwnershipResultSessionTooFresh& object) PRINT
    void operator() (td::td_api::chat& object) PRINT
    void operator() (td::td_api::chatActionTyping& object) PRINT
    void operator() (td::td_api::chatActionRecordingVideo& object) PRINT
    void operator() (td::td_api::chatActionUploadingVideo& object) PRINT
    void operator() (td::td_api::chatActionRecordingVoiceNote& object) PRINT
    void operator() (td::td_api::chatActionUploadingVoiceNote& object) PRINT
    void operator() (td::td_api::chatActionUploadingPhoto& object) PRINT
    void operator() (td::td_api::chatActionUploadingDocument& object) PRINT
    void operator() (td::td_api::chatActionChoosingSticker& object) PRINT
    void operator() (td::td_api::chatActionChoosingLocation& object) PRINT
    void operator() (td::td_api::chatActionChoosingContact& object) PRINT
    void operator() (td::td_api::chatActionStartPlayingGame& object) PRINT
    void operator() (td::td_api::chatActionRecordingVideoNote& object) PRINT
    void operator() (td::td_api::chatActionUploadingVideoNote& object) PRINT
    void operator() (td::td_api::chatActionWatchingAnimations& object) PRINT
    void operator() (td::td_api::chatActionCancel& object) PRINT
    void operator() (td::td_api::chatActionBarReportSpam& object) PRINT
    void operator() (td::td_api::chatActionBarReportUnrelatedLocation& object) PRINT
    void operator() (td::td_api::chatActionBarInviteMembers& object) PRINT
    void operator() (td::td_api::chatActionBarReportAddBlock& object) PRINT
    void operator() (td::td_api::chatActionBarAddContact& object) PRINT
    void operator() (td::td_api::chatActionBarSharePhoneNumber& object) PRINT
    void operator() (td::td_api::chatActionBarJoinRequest& object) PRINT
    void operator() (td::td_api::chatAdministrator& object) PRINT
    void operator() (td::td_api::chatAdministrators& object) PRINT
    void operator() (td::td_api::chatEvent& object) PRINT
    void operator() (td::td_api::chatEventMessageEdited& object) PRINT
    void operator() (td::td_api::chatEventMessageDeleted& object) PRINT
    void operator() (td::td_api::chatEventPollStopped& object) PRINT
    void operator() (td::td_api::chatEventMessagePinned& object) PRINT
    void operator() (td::td_api::chatEventMessageUnpinned& object) PRINT
    void operator() (td::td_api::chatEventMemberJoined& object) PRINT
    void operator() (td::td_api::chatEventMemberJoinedByInviteLink& object) PRINT
    void operator() (td::td_api::chatEventMemberJoinedByRequest& object) PRINT
    void operator() (td::td_api::chatEventMemberLeft& object) PRINT
    void operator() (td::td_api::chatEventMemberInvited& object) PRINT
    void operator() (td::td_api::chatEventMemberPromoted& object) PRINT
    void operator() (td::td_api::chatEventMemberRestricted& object) PRINT
    void operator() (td::td_api::chatEventTitleChanged& object) PRINT
    void operator() (td::td_api::chatEventPermissionsChanged& object) PRINT
    void operator() (td::td_api::chatEventDescriptionChanged& object) PRINT
    void operator() (td::td_api::chatEventUsernameChanged& object) PRINT
    void operator() (td::td_api::chatEventPhotoChanged& object) PRINT
    void operator() (td::td_api::chatEventInvitesToggled& object) PRINT
    void operator() (td::td_api::chatEventLinkedChatChanged& object) PRINT
    void operator() (td::td_api::chatEventSlowModeDelayChanged& object) PRINT
    void operator() (td::td_api::chatEventMessageTtlChanged& object) PRINT
    void operator() (td::td_api::chatEventSignMessagesToggled& object) PRINT
    void operator() (td::td_api::chatEventHasProtectedContentToggled& object) PRINT
    void operator() (td::td_api::chatEventStickerSetChanged& object) PRINT
    void operator() (td::td_api::chatEventLocationChanged& object) PRINT
    void operator() (td::td_api::chatEventIsAllHistoryAvailableToggled& object) PRINT
    void operator() (td::td_api::chatEventInviteLinkEdited& object) PRINT
    void operator() (td::td_api::chatEventInviteLinkRevoked& object) PRINT
    void operator() (td::td_api::chatEventInviteLinkDeleted& object) PRINT
    void operator() (td::td_api::chatEventVideoChatCreated& object) PRINT
    void operator() (td::td_api::chatEventVideoChatEnded& object) PRINT
    void operator() (td::td_api::chatEventVideoChatParticipantIsMutedToggled& object) PRINT
    void operator() (td::td_api::chatEventVideoChatParticipantVolumeLevelChanged& object) PRINT
    void operator() (td::td_api::chatEventVideoChatMuteNewParticipantsToggled& object) PRINT
    void operator() (td::td_api::chatEventLogFilters& object) PRINT
    void operator() (td::td_api::chatEvents& object) PRINT
    void operator() (td::td_api::chatFilter& object) PRINT
    void operator() (td::td_api::chatFilterInfo& object) PRINT
    void operator() (td::td_api::chatInviteLink& object) PRINT
    void operator() (td::td_api::chatInviteLinkCount& object) PRINT
    void operator() (td::td_api::chatInviteLinkCounts& object) PRINT
    void operator() (td::td_api::chatInviteLinkInfo& object) PRINT
    void operator() (td::td_api::chatInviteLinkMember& object) PRINT
    void operator() (td::td_api::chatInviteLinkMembers& object) PRINT
    void operator() (td::td_api::chatInviteLinks& object) PRINT
    void operator() (td::td_api::chatJoinRequest& object) PRINT
    void operator() (td::td_api::chatJoinRequests& object) PRINT
    void operator() (td::td_api::chatJoinRequestsInfo& object) PRINT
    void operator() (td::td_api::chatListMain& object) PRINT
    void operator() (td::td_api::chatListArchive& object) PRINT
    void operator() (td::td_api::chatListFilter& object) PRINT
    void operator() (td::td_api::chatLists& object) PRINT
    void operator() (td::td_api::chatLocation& object) PRINT
    void operator() (td::td_api::chatMember& object) PRINT
    void operator() (td::td_api::chatMemberStatusCreator& object) PRINT
    void operator() (td::td_api::chatMemberStatusAdministrator& object) PRINT
    void operator() (td::td_api::chatMemberStatusMember& object) PRINT
    void operator() (td::td_api::chatMemberStatusRestricted& object) PRINT
    void operator() (td::td_api::chatMemberStatusLeft& object) PRINT
    void operator() (td::td_api::chatMemberStatusBanned& object) PRINT
    void operator() (td::td_api::chatMembers& object) PRINT
    void operator() (td::td_api::chatMembersFilterContacts& object) PRINT
    void operator() (td::td_api::chatMembersFilterAdministrators& object) PRINT
    void operator() (td::td_api::chatMembersFilterMembers& object) PRINT
    void operator() (td::td_api::chatMembersFilterMention& object) PRINT
    void operator() (td::td_api::chatMembersFilterRestricted& object) PRINT
    void operator() (td::td_api::chatMembersFilterBanned& object) PRINT
    void operator() (td::td_api::chatMembersFilterBots& object) PRINT
    void operator() (td::td_api::chatNearby& object) PRINT
    void operator() (td::td_api::chatNotificationSettings& object) PRINT
    void operator() (td::td_api::chatPermissions& object) PRINT
    void operator() (td::td_api::chatPhoto& object) PRINT
    void operator() (td::td_api::chatPhotoInfo& object) PRINT
    void operator() (td::td_api::chatPhotos& object) PRINT
    void operator() (td::td_api::chatPosition& object) PRINT
    void operator() (td::td_api::chatReportReasonSpam& object) PRINT
    void operator() (td::td_api::chatReportReasonViolence& object) PRINT
    void operator() (td::td_api::chatReportReasonPornography& object) PRINT
    void operator() (td::td_api::chatReportReasonChildAbuse& object) PRINT
    void operator() (td::td_api::chatReportReasonCopyright& object) PRINT
    void operator() (td::td_api::chatReportReasonUnrelatedLocation& object) PRINT
    void operator() (td::td_api::chatReportReasonFake& object) PRINT
    void operator() (td::td_api::chatReportReasonCustom& object) PRINT
    void operator() (td::td_api::chatSourceMtprotoProxy& object) PRINT
    void operator() (td::td_api::chatSourcePublicServiceAnnouncement& object) PRINT
    void operator() (td::td_api::chatStatisticsSupergroup& object) PRINT
    void operator() (td::td_api::chatStatisticsChannel& object) PRINT
    void operator() (td::td_api::chatStatisticsAdministratorActionsInfo& object) PRINT
    void operator() (td::td_api::chatStatisticsInviterInfo& object) PRINT
    void operator() (td::td_api::chatStatisticsMessageInteractionInfo& object) PRINT
    void operator() (td::td_api::chatStatisticsMessageSenderInfo& object) PRINT
    void operator() (td::td_api::chatTheme& object) PRINT
    void operator() (td::td_api::chatTypePrivate& object) PRINT
    void operator() (td::td_api::chatTypeBasicGroup& object) PRINT
    void operator() (td::td_api::chatTypeSupergroup& object) PRINT
    void operator() (td::td_api::chatTypeSecret& object) PRINT
    void operator() (td::td_api::chats& object) PRINT
    void operator() (td::td_api::chatsNearby& object) PRINT
    void operator() (td::td_api::checkChatUsernameResultOk& object) PRINT
    void operator() (td::td_api::checkChatUsernameResultUsernameInvalid& object) PRINT
    void operator() (td::td_api::checkChatUsernameResultUsernameOccupied& object) PRINT
    void operator() (td::td_api::checkChatUsernameResultPublicChatsTooMuch& object) PRINT
    void operator() (td::td_api::checkChatUsernameResultPublicGroupsUnavailable& object) PRINT
    void operator() (td::td_api::checkStickerSetNameResultOk& object) PRINT
    void operator() (td::td_api::checkStickerSetNameResultNameInvalid& object) PRINT
    void operator() (td::td_api::checkStickerSetNameResultNameOccupied& object) PRINT
    void operator() (td::td_api::closedVectorPath& object) PRINT
    void operator() (td::td_api::connectedWebsite& object) PRINT
    void operator() (td::td_api::connectedWebsites& object) PRINT
    void operator() (td::td_api::connectionStateWaitingForNetwork& object) PRINT
    void operator() (td::td_api::connectionStateConnectingToProxy& object) PRINT
    void operator() (td::td_api::connectionStateConnecting& object) PRINT
    void operator() (td::td_api::connectionStateUpdating& object) PRINT
    void operator() (td::td_api::connectionStateReady& object) PRINT
    void operator() (td::td_api::contact& object) PRINT
    void operator() (td::td_api::count& object) PRINT
    void operator() (td::td_api::countries& object) PRINT
    void operator() (td::td_api::countryInfo& object) PRINT
    void operator() (td::td_api::customRequestResult& object) PRINT
    void operator() (td::td_api::databaseStatistics& object) PRINT
    void operator() (td::td_api::date& object) PRINT
    void operator() (td::td_api::dateRange& object) PRINT
    void operator() (td::td_api::datedFile& object) PRINT
    void operator() (td::td_api::deepLinkInfo& object) PRINT
    void operator() (td::td_api::deviceTokenFirebaseCloudMessaging& object) PRINT
    void operator() (td::td_api::deviceTokenApplePush& object) PRINT
    void operator() (td::td_api::deviceTokenApplePushVoIP& object) PRINT
    void operator() (td::td_api::deviceTokenWindowsPush& object) PRINT
    void operator() (td::td_api::deviceTokenMicrosoftPush& object) PRINT
    void operator() (td::td_api::deviceTokenMicrosoftPushVoIP& object) PRINT
    void operator() (td::td_api::deviceTokenWebPush& object) PRINT
    void operator() (td::td_api::deviceTokenSimplePush& object) PRINT
    void operator() (td::td_api::deviceTokenUbuntuPush& object) PRINT
    void operator() (td::td_api::deviceTokenBlackBerryPush& object) PRINT
    void operator() (td::td_api::deviceTokenTizenPush& object) PRINT
    void operator() (td::td_api::diceStickersRegular& object) PRINT
    void operator() (td::td_api::diceStickersSlotMachine& object) PRINT
    void operator() (td::td_api::document& object) PRINT
    void operator() (td::td_api::draftMessage& object) PRINT
    void operator() (td::td_api::emailAddressAuthenticationCodeInfo& object) PRINT
    void operator() (td::td_api::emojis& object) PRINT
    void operator() (td::td_api::encryptedCredentials& object) PRINT
    void operator() (td::td_api::encryptedPassportElement& object) PRINT
    void operator() (td::td_api::error& object) PRINT
    void operator() (td::td_api::file& object) PRINT
    void operator() (td::td_api::filePart& object) PRINT
    void operator() (td::td_api::fileTypeNone& object) PRINT
    void operator() (td::td_api::fileTypeAnimation& object) PRINT
    void operator() (td::td_api::fileTypeAudio& object) PRINT
    void operator() (td::td_api::fileTypeDocument& object) PRINT
    void operator() (td::td_api::fileTypePhoto& object) PRINT
    void operator() (td::td_api::fileTypeProfilePhoto& object) PRINT
    void operator() (td::td_api::fileTypeSecret& object) PRINT
    void operator() (td::td_api::fileTypeSecretThumbnail& object) PRINT
    void operator() (td::td_api::fileTypeSecure& object) PRINT
    void operator() (td::td_api::fileTypeSticker& object) PRINT
    void operator() (td::td_api::fileTypeThumbnail& object) PRINT
    void operator() (td::td_api::fileTypeUnknown& object) PRINT
    void operator() (td::td_api::fileTypeVideo& object) PRINT
    void operator() (td::td_api::fileTypeVideoNote& object) PRINT
    void operator() (td::td_api::fileTypeVoiceNote& object) PRINT
    void operator() (td::td_api::fileTypeWallpaper& object) PRINT
    void operator() (td::td_api::formattedText& object) PRINT
    void operator() (td::td_api::foundMessages& object) PRINT
    void operator() (td::td_api::game& object) PRINT
    void operator() (td::td_api::gameHighScore& object) PRINT
    void operator() (td::td_api::gameHighScores& object) PRINT
    void operator() (td::td_api::groupCall& object) PRINT
    void operator() (td::td_api::groupCallId& object) PRINT
    void operator() (td::td_api::groupCallParticipant& object) PRINT
    void operator() (td::td_api::groupCallParticipantVideoInfo& object) PRINT
    void operator() (td::td_api::groupCallRecentSpeaker& object) PRINT
    void operator() (td::td_api::groupCallVideoQualityThumbnail& object) PRINT
    void operator() (td::td_api::groupCallVideoQualityMedium& object) PRINT
    void operator() (td::td_api::groupCallVideoQualityFull& object) PRINT
    void operator() (td::td_api::groupCallVideoSourceGroup& object) PRINT
    void operator() (td::td_api::hashtags& object) PRINT
    void operator() (td::td_api::httpUrl& object) PRINT
    void operator() (td::td_api::identityDocument& object) PRINT
    void operator() (td::td_api::importedContacts& object) PRINT
    void operator() (td::td_api::inlineKeyboardButton& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeUrl& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeLoginUrl& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeCallback& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeCallbackWithPassword& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeCallbackGame& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeSwitchInline& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeBuy& object) PRINT
    void operator() (td::td_api::inlineKeyboardButtonTypeUser& object) PRINT
    void operator() (td::td_api::inlineQueryResultArticle& object) PRINT
    void operator() (td::td_api::inlineQueryResultContact& object) PRINT
    void operator() (td::td_api::inlineQueryResultLocation& object) PRINT
    void operator() (td::td_api::inlineQueryResultVenue& object) PRINT
    void operator() (td::td_api::inlineQueryResultGame& object) PRINT
    void operator() (td::td_api::inlineQueryResultAnimation& object) PRINT
    void operator() (td::td_api::inlineQueryResultAudio& object) PRINT
    void operator() (td::td_api::inlineQueryResultDocument& object) PRINT
    void operator() (td::td_api::inlineQueryResultPhoto& object) PRINT
    void operator() (td::td_api::inlineQueryResultSticker& object) PRINT
    void operator() (td::td_api::inlineQueryResultVideo& object) PRINT
    void operator() (td::td_api::inlineQueryResultVoiceNote& object) PRINT
    void operator() (td::td_api::inlineQueryResults& object) PRINT
    void operator() (td::td_api::inputBackgroundLocal& object) PRINT
    void operator() (td::td_api::inputBackgroundRemote& object) PRINT
    void operator() (td::td_api::inputChatPhotoPrevious& object) PRINT
    void operator() (td::td_api::inputChatPhotoStatic& object) PRINT
    void operator() (td::td_api::inputChatPhotoAnimation& object) PRINT
    void operator() (td::td_api::inputCredentialsSaved& object) PRINT
    void operator() (td::td_api::inputCredentialsNew& object) PRINT
    void operator() (td::td_api::inputCredentialsApplePay& object) PRINT
    void operator() (td::td_api::inputCredentialsGooglePay& object) PRINT
    void operator() (td::td_api::inputFileId& object) PRINT
    void operator() (td::td_api::inputFileRemote& object) PRINT
    void operator() (td::td_api::inputFileLocal& object) PRINT
    void operator() (td::td_api::inputFileGenerated& object) PRINT
    void operator() (td::td_api::inputIdentityDocument& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultAnimation& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultArticle& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultAudio& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultContact& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultDocument& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultGame& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultLocation& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultPhoto& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultSticker& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultVenue& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultVideo& object) PRINT
    void operator() (td::td_api::inputInlineQueryResultVoiceNote& object) PRINT
    void operator() (td::td_api::inputMessageText& object) PRINT
    void operator() (td::td_api::inputMessageAnimation& object) PRINT
    void operator() (td::td_api::inputMessageAudio& object) PRINT
    void operator() (td::td_api::inputMessageDocument& object) PRINT
    void operator() (td::td_api::inputMessagePhoto& object) PRINT
    void operator() (td::td_api::inputMessageSticker& object) PRINT
    void operator() (td::td_api::inputMessageVideo& object) PRINT
    void operator() (td::td_api::inputMessageVideoNote& object) PRINT
    void operator() (td::td_api::inputMessageVoiceNote& object) PRINT
    void operator() (td::td_api::inputMessageLocation& object) PRINT
    void operator() (td::td_api::inputMessageVenue& object) PRINT
    void operator() (td::td_api::inputMessageContact& object) PRINT
    void operator() (td::td_api::inputMessageDice& object) PRINT
    void operator() (td::td_api::inputMessageGame& object) PRINT
    void operator() (td::td_api::inputMessageInvoice& object) PRINT
    void operator() (td::td_api::inputMessagePoll& object) PRINT
    void operator() (td::td_api::inputMessageForwarded& object) PRINT
    void operator() (td::td_api::inputPassportElementPersonalDetails& object) PRINT
    void operator() (td::td_api::inputPassportElementPassport& object) PRINT
    void operator() (td::td_api::inputPassportElementDriverLicense& object) PRINT
    void operator() (td::td_api::inputPassportElementIdentityCard& object) PRINT
    void operator() (td::td_api::inputPassportElementInternalPassport& object) PRINT
    void operator() (td::td_api::inputPassportElementAddress& object) PRINT
    void operator() (td::td_api::inputPassportElementUtilityBill& object) PRINT
    void operator() (td::td_api::inputPassportElementBankStatement& object) PRINT
    void operator() (td::td_api::inputPassportElementRentalAgreement& object) PRINT
    void operator() (td::td_api::inputPassportElementPassportRegistration& object) PRINT
    void operator() (td::td_api::inputPassportElementTemporaryRegistration& object) PRINT
    void operator() (td::td_api::inputPassportElementPhoneNumber& object) PRINT
    void operator() (td::td_api::inputPassportElementEmailAddress& object) PRINT
    void operator() (td::td_api::inputPassportElementError& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceUnspecified& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceDataField& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceFrontSide& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceReverseSide& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceSelfie& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceTranslationFile& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceTranslationFiles& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceFile& object) PRINT
    void operator() (td::td_api::inputPassportElementErrorSourceFiles& object) PRINT
    void operator() (td::td_api::inputPersonalDocument& object) PRINT
    void operator() (td::td_api::inputStickerStatic& object) PRINT
    void operator() (td::td_api::inputStickerAnimated& object) PRINT
    void operator() (td::td_api::inputThumbnail& object) PRINT
    void operator() (td::td_api::internalLinkTypeActiveSessions& object) PRINT
    void operator() (td::td_api::internalLinkTypeAuthenticationCode& object) PRINT
    void operator() (td::td_api::internalLinkTypeBackground& object) PRINT
    void operator() (td::td_api::internalLinkTypeBotStart& object) PRINT
    void operator() (td::td_api::internalLinkTypeBotStartInGroup& object) PRINT
    void operator() (td::td_api::internalLinkTypeChangePhoneNumber& object) PRINT
    void operator() (td::td_api::internalLinkTypeChatInvite& object) PRINT
    void operator() (td::td_api::internalLinkTypeFilterSettings& object) PRINT
    void operator() (td::td_api::internalLinkTypeGame& object) PRINT
    void operator() (td::td_api::internalLinkTypeLanguagePack& object) PRINT
    void operator() (td::td_api::internalLinkTypeMessage& object) PRINT
    void operator() (td::td_api::internalLinkTypeMessageDraft& object) PRINT
    void operator() (td::td_api::internalLinkTypePassportDataRequest& object) PRINT
    void operator() (td::td_api::internalLinkTypePhoneNumberConfirmation& object) PRINT
    void operator() (td::td_api::internalLinkTypeProxy& object) PRINT
    void operator() (td::td_api::internalLinkTypePublicChat& object) PRINT
    void operator() (td::td_api::internalLinkTypeQrCodeAuthentication& object) PRINT
    void operator() (td::td_api::internalLinkTypeSettings& object) PRINT
    void operator() (td::td_api::internalLinkTypeStickerSet& object) PRINT
    void operator() (td::td_api::internalLinkTypeTheme& object) PRINT
    void operator() (td::td_api::internalLinkTypeThemeSettings& object) PRINT
    void operator() (td::td_api::internalLinkTypeUnknownDeepLink& object) PRINT
    void operator() (td::td_api::internalLinkTypeUnsupportedProxy& object) PRINT
    void operator() (td::td_api::internalLinkTypeVideoChat& object) PRINT
    void operator() (td::td_api::invoice& object) PRINT
    void operator() (td::td_api::jsonObjectMember& object) PRINT
    void operator() (td::td_api::jsonValueNull& object) PRINT
    void operator() (td::td_api::jsonValueBoolean& object) PRINT
    void operator() (td::td_api::jsonValueNumber& object) PRINT
    void operator() (td::td_api::jsonValueString& object) PRINT
    void operator() (td::td_api::jsonValueArray& object) PRINT
    void operator() (td::td_api::jsonValueObject& object) PRINT
    void operator() (td::td_api::keyboardButton& object) PRINT
    void operator() (td::td_api::keyboardButtonTypeText& object) PRINT
    void operator() (td::td_api::keyboardButtonTypeRequestPhoneNumber& object) PRINT
    void operator() (td::td_api::keyboardButtonTypeRequestLocation& object) PRINT
    void operator() (td::td_api::keyboardButtonTypeRequestPoll& object) PRINT
    void operator() (td::td_api::labeledPricePart& object) PRINT
    void operator() (td::td_api::languagePackInfo& object) PRINT
    void operator() (td::td_api::languagePackString& object) PRINT
    void operator() (td::td_api::languagePackStringValueOrdinary& object) PRINT
    void operator() (td::td_api::languagePackStringValuePluralized& object) PRINT
    void operator() (td::td_api::languagePackStringValueDeleted& object) PRINT
    void operator() (td::td_api::languagePackStrings& object) PRINT
    void operator() (td::td_api::localFile& object) PRINT
    void operator() (td::td_api::localizationTargetInfo& object) PRINT
    void operator() (td::td_api::location& object) PRINT
    void operator() (td::td_api::logStreamDefault& object) PRINT
    void operator() (td::td_api::logStreamFile& object) PRINT
    void operator() (td::td_api::logStreamEmpty& object) PRINT
    void operator() (td::td_api::logTags& object) PRINT
    void operator() (td::td_api::logVerbosityLevel& object) PRINT
    void operator() (td::td_api::loginUrlInfoOpen& object) PRINT
    void operator() (td::td_api::loginUrlInfoRequestConfirmation& object) PRINT
    void operator() (td::td_api::maskPointForehead& object) PRINT
    void operator() (td::td_api::maskPointEyes& object) PRINT
    void operator() (td::td_api::maskPointMouth& object) PRINT
    void operator() (td::td_api::maskPointChin& object) PRINT
    void operator() (td::td_api::maskPosition& object) PRINT
    void operator() (td::td_api::message& object) PRINT
    void operator() (td::td_api::messageCalendar& object) PRINT
    void operator() (td::td_api::messageCalendarDay& object) PRINT
    void operator() (td::td_api::messageText& object) PRINT
    void operator() (td::td_api::messageAnimation& object) PRINT
    void operator() (td::td_api::messageAudio& object) PRINT
    void operator() (td::td_api::messageDocument& object) PRINT
    void operator() (td::td_api::messagePhoto& object) PRINT
    void operator() (td::td_api::messageExpiredPhoto& object) PRINT
    void operator() (td::td_api::messageSticker& object) PRINT
    void operator() (td::td_api::messageVideo& object) PRINT
    void operator() (td::td_api::messageExpiredVideo& object) PRINT
    void operator() (td::td_api::messageVideoNote& object) PRINT
    void operator() (td::td_api::messageVoiceNote& object) PRINT
    void operator() (td::td_api::messageLocation& object) PRINT
    void operator() (td::td_api::messageVenue& object) PRINT
    void operator() (td::td_api::messageContact& object) PRINT
    void operator() (td::td_api::messageAnimatedEmoji& object) PRINT
    void operator() (td::td_api::messageDice& object) PRINT
    void operator() (td::td_api::messageGame& object) PRINT
    void operator() (td::td_api::messagePoll& object) PRINT
    void operator() (td::td_api::messageInvoice& object) PRINT
    void operator() (td::td_api::messageCall& object) PRINT
    void operator() (td::td_api::messageVideoChatScheduled& object) PRINT
    void operator() (td::td_api::messageVideoChatStarted& object) PRINT
    void operator() (td::td_api::messageVideoChatEnded& object) PRINT
    void operator() (td::td_api::messageInviteVideoChatParticipants& object) PRINT
    void operator() (td::td_api::messageBasicGroupChatCreate& object) PRINT
    void operator() (td::td_api::messageSupergroupChatCreate& object) PRINT
    void operator() (td::td_api::messageChatChangeTitle& object) PRINT
    void operator() (td::td_api::messageChatChangePhoto& object) PRINT
    void operator() (td::td_api::messageChatDeletePhoto& object) PRINT
    void operator() (td::td_api::messageChatAddMembers& object) PRINT
    void operator() (td::td_api::messageChatJoinByLink& object) PRINT
    void operator() (td::td_api::messageChatJoinByRequest& object) PRINT
    void operator() (td::td_api::messageChatDeleteMember& object) PRINT
    void operator() (td::td_api::messageChatUpgradeTo& object) PRINT
    void operator() (td::td_api::messageChatUpgradeFrom& object) PRINT
    void operator() (td::td_api::messagePinMessage& object) PRINT
    void operator() (td::td_api::messageScreenshotTaken& object) PRINT
    void operator() (td::td_api::messageChatSetTheme& object) PRINT
    void operator() (td::td_api::messageChatSetTtl& object) PRINT
    void operator() (td::td_api::messageCustomServiceAction& object) PRINT
    void operator() (td::td_api::messageGameScore& object) PRINT
    void operator() (td::td_api::messagePaymentSuccessful& object) PRINT
    void operator() (td::td_api::messagePaymentSuccessfulBot& object) PRINT
    void operator() (td::td_api::messageContactRegistered& object) PRINT
    void operator() (td::td_api::messageWebsiteConnected& object) PRINT
    void operator() (td::td_api::messagePassportDataSent& object) PRINT
    void operator() (td::td_api::messagePassportDataReceived& object) PRINT
    void operator() (td::td_api::messageProximityAlertTriggered& object) PRINT
    void operator() (td::td_api::messageUnsupported& object) PRINT
    void operator() (td::td_api::messageCopyOptions& object) PRINT
    void operator() (td::td_api::messageFileTypePrivate& object) PRINT
    void operator() (td::td_api::messageFileTypeGroup& object) PRINT
    void operator() (td::td_api::messageFileTypeUnknown& object) PRINT
    void operator() (td::td_api::messageForwardInfo& object) PRINT
    void operator() (td::td_api::messageForwardOriginUser& object) PRINT
    void operator() (td::td_api::messageForwardOriginChat& object) PRINT
    void operator() (td::td_api::messageForwardOriginHiddenUser& object) PRINT
    void operator() (td::td_api::messageForwardOriginChannel& object) PRINT
    void operator() (td::td_api::messageForwardOriginMessageImport& object) PRINT
    void operator() (td::td_api::messageInteractionInfo& object) PRINT
    void operator() (td::td_api::messageLink& object) PRINT
    void operator() (td::td_api::messageLinkInfo& object) PRINT
    void operator() (td::td_api::messagePosition& object) PRINT
    void operator() (td::td_api::messagePositions& object) PRINT
    void operator() (td::td_api::messageReplyInfo& object) PRINT
    void operator() (td::td_api::messageSchedulingStateSendAtDate& object) PRINT
    void operator() (td::td_api::messageSchedulingStateSendWhenOnline& object) PRINT
    void operator() (td::td_api::messageSendOptions& object) PRINT
    void operator() (td::td_api::messageSenderUser& object) PRINT
    void operator() (td::td_api::messageSenderChat& object) PRINT
    void operator() (td::td_api::messageSenders& object) PRINT
    void operator() (td::td_api::messageSendingStatePending& object) PRINT
    void operator() (td::td_api::messageSendingStateFailed& object) PRINT
    void operator() (td::td_api::messageStatistics& object) PRINT
    void operator() (td::td_api::messageThreadInfo& object) PRINT
    void operator() (td::td_api::messages& object) PRINT
    void operator() (td::td_api::minithumbnail& object) PRINT
    void operator() (td::td_api::networkStatistics& object) PRINT
    void operator() (td::td_api::networkStatisticsEntryFile& object) PRINT
    void operator() (td::td_api::networkStatisticsEntryCall& object) PRINT
    void operator() (td::td_api::networkTypeNone& object) PRINT
    void operator() (td::td_api::networkTypeMobile& object) PRINT
    void operator() (td::td_api::networkTypeMobileRoaming& object) PRINT
    void operator() (td::td_api::networkTypeWiFi& object) PRINT
    void operator() (td::td_api::networkTypeOther& object) PRINT
    void operator() (td::td_api::notification& object) PRINT
    void operator() (td::td_api::notificationGroup& object) PRINT
    void operator() (td::td_api::notificationGroupTypeMessages& object) PRINT
    void operator() (td::td_api::notificationGroupTypeMentions& object) PRINT
    void operator() (td::td_api::notificationGroupTypeSecretChat& object) PRINT
    void operator() (td::td_api::notificationGroupTypeCalls& object) PRINT
    void operator() (td::td_api::notificationSettingsScopePrivateChats& object) PRINT
    void operator() (td::td_api::notificationSettingsScopeGroupChats& object) PRINT
    void operator() (td::td_api::notificationSettingsScopeChannelChats& object) PRINT
    void operator() (td::td_api::notificationTypeNewMessage& object) PRINT
    void operator() (td::td_api::notificationTypeNewSecretChat& object) PRINT
    void operator() (td::td_api::notificationTypeNewCall& object) PRINT
    void operator() (td::td_api::notificationTypeNewPushMessage& object) PRINT
    void operator() (td::td_api::ok& object) PRINT
    void operator() (td::td_api::optionValueBoolean& object) PRINT
    void operator() (td::td_api::optionValueEmpty& object) PRINT
    void operator() (td::td_api::optionValueInteger& object) PRINT
    void operator() (td::td_api::optionValueString& object) PRINT
    void operator() (td::td_api::orderInfo& object) PRINT
    void operator() (td::td_api::pageBlockTitle& object) PRINT
    void operator() (td::td_api::pageBlockSubtitle& object) PRINT
    void operator() (td::td_api::pageBlockAuthorDate& object) PRINT
    void operator() (td::td_api::pageBlockHeader& object) PRINT
    void operator() (td::td_api::pageBlockSubheader& object) PRINT
    void operator() (td::td_api::pageBlockKicker& object) PRINT
    void operator() (td::td_api::pageBlockParagraph& object) PRINT
    void operator() (td::td_api::pageBlockPreformatted& object) PRINT
    void operator() (td::td_api::pageBlockFooter& object) PRINT
    void operator() (td::td_api::pageBlockDivider& object) PRINT
    void operator() (td::td_api::pageBlockAnchor& object) PRINT
    void operator() (td::td_api::pageBlockList& object) PRINT
    void operator() (td::td_api::pageBlockBlockQuote& object) PRINT
    void operator() (td::td_api::pageBlockPullQuote& object) PRINT
    void operator() (td::td_api::pageBlockAnimation& object) PRINT
    void operator() (td::td_api::pageBlockAudio& object) PRINT
    void operator() (td::td_api::pageBlockPhoto& object) PRINT
    void operator() (td::td_api::pageBlockVideo& object) PRINT
    void operator() (td::td_api::pageBlockVoiceNote& object) PRINT
    void operator() (td::td_api::pageBlockCover& object) PRINT
    void operator() (td::td_api::pageBlockEmbedded& object) PRINT
    void operator() (td::td_api::pageBlockEmbeddedPost& object) PRINT
    void operator() (td::td_api::pageBlockCollage& object) PRINT
    void operator() (td::td_api::pageBlockSlideshow& object) PRINT
    void operator() (td::td_api::pageBlockChatLink& object) PRINT
    void operator() (td::td_api::pageBlockTable& object) PRINT
    void operator() (td::td_api::pageBlockDetails& object) PRINT
    void operator() (td::td_api::pageBlockRelatedArticles& object) PRINT
    void operator() (td::td_api::pageBlockMap& object) PRINT
    void operator() (td::td_api::pageBlockCaption& object) PRINT
    void operator() (td::td_api::pageBlockHorizontalAlignmentLeft& object) PRINT
    void operator() (td::td_api::pageBlockHorizontalAlignmentCenter& object) PRINT
    void operator() (td::td_api::pageBlockHorizontalAlignmentRight& object) PRINT
    void operator() (td::td_api::pageBlockListItem& object) PRINT
    void operator() (td::td_api::pageBlockRelatedArticle& object) PRINT
    void operator() (td::td_api::pageBlockTableCell& object) PRINT
    void operator() (td::td_api::pageBlockVerticalAlignmentTop& object) PRINT
    void operator() (td::td_api::pageBlockVerticalAlignmentMiddle& object) PRINT
    void operator() (td::td_api::pageBlockVerticalAlignmentBottom& object) PRINT
    void operator() (td::td_api::passportAuthorizationForm& object) PRINT
    void operator() (td::td_api::passportElementPersonalDetails& object) PRINT
    void operator() (td::td_api::passportElementPassport& object) PRINT
    void operator() (td::td_api::passportElementDriverLicense& object) PRINT
    void operator() (td::td_api::passportElementIdentityCard& object) PRINT
    void operator() (td::td_api::passportElementInternalPassport& object) PRINT
    void operator() (td::td_api::passportElementAddress& object) PRINT
    void operator() (td::td_api::passportElementUtilityBill& object) PRINT
    void operator() (td::td_api::passportElementBankStatement& object) PRINT
    void operator() (td::td_api::passportElementRentalAgreement& object) PRINT
    void operator() (td::td_api::passportElementPassportRegistration& object) PRINT
    void operator() (td::td_api::passportElementTemporaryRegistration& object) PRINT
    void operator() (td::td_api::passportElementPhoneNumber& object) PRINT
    void operator() (td::td_api::passportElementEmailAddress& object) PRINT
    void operator() (td::td_api::passportElementError& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceUnspecified& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceDataField& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceFrontSide& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceReverseSide& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceSelfie& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceTranslationFile& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceTranslationFiles& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceFile& object) PRINT
    void operator() (td::td_api::passportElementErrorSourceFiles& object) PRINT
    void operator() (td::td_api::passportElementTypePersonalDetails& object) PRINT
    void operator() (td::td_api::passportElementTypePassport& object) PRINT
    void operator() (td::td_api::passportElementTypeDriverLicense& object) PRINT
    void operator() (td::td_api::passportElementTypeIdentityCard& object) PRINT
    void operator() (td::td_api::passportElementTypeInternalPassport& object) PRINT
    void operator() (td::td_api::passportElementTypeAddress& object) PRINT
    void operator() (td::td_api::passportElementTypeUtilityBill& object) PRINT
    void operator() (td::td_api::passportElementTypeBankStatement& object) PRINT
    void operator() (td::td_api::passportElementTypeRentalAgreement& object) PRINT
    void operator() (td::td_api::passportElementTypePassportRegistration& object) PRINT
    void operator() (td::td_api::passportElementTypeTemporaryRegistration& object) PRINT
    void operator() (td::td_api::passportElementTypePhoneNumber& object) PRINT
    void operator() (td::td_api::passportElementTypeEmailAddress& object) PRINT
    void operator() (td::td_api::passportElements& object) PRINT
    void operator() (td::td_api::passportElementsWithErrors& object) PRINT
    void operator() (td::td_api::passportRequiredElement& object) PRINT
    void operator() (td::td_api::passportSuitableElement& object) PRINT
    void operator() (td::td_api::passwordState& object) PRINT
    void operator() (td::td_api::paymentForm& object) PRINT
    void operator() (td::td_api::paymentFormTheme& object) PRINT
    void operator() (td::td_api::paymentReceipt& object) PRINT
    void operator() (td::td_api::paymentResult& object) PRINT
    void operator() (td::td_api::paymentsProviderStripe& object) PRINT
    void operator() (td::td_api::personalDetails& object) PRINT
    void operator() (td::td_api::personalDocument& object) PRINT
    void operator() (td::td_api::phoneNumberAuthenticationSettings& object) PRINT
    void operator() (td::td_api::phoneNumberInfo& object) PRINT
    void operator() (td::td_api::photo& object) PRINT
    void operator() (td::td_api::photoSize& object) PRINT
    void operator() (td::td_api::point& object) PRINT
    void operator() (td::td_api::poll& object) PRINT
    void operator() (td::td_api::pollOption& object) PRINT
    void operator() (td::td_api::pollTypeRegular& object) PRINT
    void operator() (td::td_api::pollTypeQuiz& object) PRINT
    void operator() (td::td_api::profilePhoto& object) PRINT
    void operator() (td::td_api::proxies& object) PRINT
    void operator() (td::td_api::proxy& object) PRINT
    void operator() (td::td_api::proxyTypeSocks5& object) PRINT
    void operator() (td::td_api::proxyTypeHttp& object) PRINT
    void operator() (td::td_api::proxyTypeMtproto& object) PRINT
    void operator() (td::td_api::publicChatTypeHasUsername& object) PRINT
    void operator() (td::td_api::publicChatTypeIsLocationBased& object) PRINT
    void operator() (td::td_api::pushMessageContentHidden& object) PRINT
    void operator() (td::td_api::pushMessageContentAnimation& object) PRINT
    void operator() (td::td_api::pushMessageContentAudio& object) PRINT
    void operator() (td::td_api::pushMessageContentContact& object) PRINT
    void operator() (td::td_api::pushMessageContentContactRegistered& object) PRINT
    void operator() (td::td_api::pushMessageContentDocument& object) PRINT
    void operator() (td::td_api::pushMessageContentGame& object) PRINT
    void operator() (td::td_api::pushMessageContentGameScore& object) PRINT
    void operator() (td::td_api::pushMessageContentInvoice& object) PRINT
    void operator() (td::td_api::pushMessageContentLocation& object) PRINT
    void operator() (td::td_api::pushMessageContentPhoto& object) PRINT
    void operator() (td::td_api::pushMessageContentPoll& object) PRINT
    void operator() (td::td_api::pushMessageContentScreenshotTaken& object) PRINT
    void operator() (td::td_api::pushMessageContentSticker& object) PRINT
    void operator() (td::td_api::pushMessageContentText& object) PRINT
    void operator() (td::td_api::pushMessageContentVideo& object) PRINT
    void operator() (td::td_api::pushMessageContentVideoNote& object) PRINT
    void operator() (td::td_api::pushMessageContentVoiceNote& object) PRINT
    void operator() (td::td_api::pushMessageContentBasicGroupChatCreate& object) PRINT
    void operator() (td::td_api::pushMessageContentChatAddMembers& object) PRINT
    void operator() (td::td_api::pushMessageContentChatChangePhoto& object) PRINT
    void operator() (td::td_api::pushMessageContentChatChangeTitle& object) PRINT
    void operator() (td::td_api::pushMessageContentChatSetTheme& object) PRINT
    void operator() (td::td_api::pushMessageContentChatDeleteMember& object) PRINT
    void operator() (td::td_api::pushMessageContentChatJoinByLink& object) PRINT
    void operator() (td::td_api::pushMessageContentChatJoinByRequest& object) PRINT
    void operator() (td::td_api::pushMessageContentMessageForwards& object) PRINT
    void operator() (td::td_api::pushMessageContentMediaAlbum& object) PRINT
    void operator() (td::td_api::pushReceiverId& object) PRINT
    void operator() (td::td_api::recommendedChatFilter& object) PRINT
    void operator() (td::td_api::recommendedChatFilters& object) PRINT
    void operator() (td::td_api::recoveryEmailAddress& object) PRINT
    void operator() (td::td_api::remoteFile& object) PRINT
    void operator() (td::td_api::replyMarkupRemoveKeyboard& object) PRINT
    void operator() (td::td_api::replyMarkupForceReply& object) PRINT
    void operator() (td::td_api::replyMarkupShowKeyboard& object) PRINT
    void operator() (td::td_api::replyMarkupInlineKeyboard& object) PRINT
    void operator() (td::td_api::resetPasswordResultOk& object) PRINT
    void operator() (td::td_api::resetPasswordResultPending& object) PRINT
    void operator() (td::td_api::resetPasswordResultDeclined& object) PRINT
    void operator() (td::td_api::richTextPlain& object) PRINT
    void operator() (td::td_api::richTextBold& object) PRINT
    void operator() (td::td_api::richTextItalic& object) PRINT
    void operator() (td::td_api::richTextUnderline& object) PRINT
    void operator() (td::td_api::richTextStrikethrough& object) PRINT
    void operator() (td::td_api::richTextFixed& object) PRINT
    void operator() (td::td_api::richTextUrl& object) PRINT
    void operator() (td::td_api::richTextEmailAddress& object) PRINT
    void operator() (td::td_api::richTextSubscript& object) PRINT
    void operator() (td::td_api::richTextSuperscript& object) PRINT
    void operator() (td::td_api::richTextMarked& object) PRINT
    void operator() (td::td_api::richTextPhoneNumber& object) PRINT
    void operator() (td::td_api::richTextIcon& object) PRINT
    void operator() (td::td_api::richTextReference& object) PRINT
    void operator() (td::td_api::richTextAnchor& object) PRINT
    void operator() (td::td_api::richTextAnchorLink& object) PRINT
    void operator() (td::td_api::richTexts& object) PRINT
    void operator() (td::td_api::savedCredentials& object) PRINT
    void operator() (td::td_api::scopeNotificationSettings& object) PRINT
    void operator() (td::td_api::searchMessagesFilterEmpty& object) PRINT
    void operator() (td::td_api::searchMessagesFilterAnimation& object) PRINT
    void operator() (td::td_api::searchMessagesFilterAudio& object) PRINT
    void operator() (td::td_api::searchMessagesFilterDocument& object) PRINT
    void operator() (td::td_api::searchMessagesFilterPhoto& object) PRINT
    void operator() (td::td_api::searchMessagesFilterVideo& object) PRINT
    void operator() (td::td_api::searchMessagesFilterVoiceNote& object) PRINT
    void operator() (td::td_api::searchMessagesFilterPhotoAndVideo& object) PRINT
    void operator() (td::td_api::searchMessagesFilterUrl& object) PRINT
    void operator() (td::td_api::searchMessagesFilterChatPhoto& object) PRINT
    void operator() (td::td_api::searchMessagesFilterVideoNote& object) PRINT
    void operator() (td::td_api::searchMessagesFilterVoiceAndVideoNote& object) PRINT
    void operator() (td::td_api::searchMessagesFilterMention& object) PRINT
    void operator() (td::td_api::searchMessagesFilterUnreadMention& object) PRINT
    void operator() (td::td_api::searchMessagesFilterFailedToSend& object) PRINT
    void operator() (td::td_api::searchMessagesFilterPinned& object) PRINT
    void operator() (td::td_api::seconds& object) PRINT
    void operator() (td::td_api::secretChat& object) PRINT
    void operator() (td::td_api::secretChatStatePending& object) PRINT
    void operator() (td::td_api::secretChatStateReady& object) PRINT
    void operator() (td::td_api::secretChatStateClosed& object) PRINT
    void operator() (td::td_api::session& object) PRINT
    void operator() (td::td_api::sessions& object) PRINT
    void operator() (td::td_api::shippingOption& object) PRINT
    void operator() (td::td_api::sponsoredMessage& object) PRINT
    void operator() (td::td_api::statisticalGraphData& object) PRINT
    void operator() (td::td_api::statisticalGraphAsync& object) PRINT
    void operator() (td::td_api::statisticalGraphError& object) PRINT
    void operator() (td::td_api::statisticalValue& object) PRINT
    void operator() (td::td_api::sticker& object) PRINT
    void operator() (td::td_api::stickerSet& object) PRINT
    void operator() (td::td_api::stickerSetInfo& object) PRINT
    void operator() (td::td_api::stickerSets& object) PRINT
    void operator() (td::td_api::stickers& object) PRINT
    void operator() (td::td_api::storageStatistics& object) PRINT
    void operator() (td::td_api::storageStatisticsByChat& object) PRINT
    void operator() (td::td_api::storageStatisticsByFileType& object) PRINT
    void operator() (td::td_api::storageStatisticsFast& object) PRINT
    void operator() (td::td_api::suggestedActionEnableArchiveAndMuteNewChats& object) PRINT
    void operator() (td::td_api::suggestedActionCheckPassword& object) PRINT
    void operator() (td::td_api::suggestedActionCheckPhoneNumber& object) PRINT
    void operator() (td::td_api::suggestedActionViewChecksHint& object) PRINT
    void operator() (td::td_api::suggestedActionConvertToBroadcastGroup& object) PRINT
    void operator() (td::td_api::suggestedActionSetPassword& object) PRINT
    void operator() (td::td_api::supergroup& object) PRINT
    void operator() (td::td_api::supergroupFullInfo& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterRecent& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterContacts& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterAdministrators& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterSearch& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterRestricted& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterBanned& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterMention& object) PRINT
    void operator() (td::td_api::supergroupMembersFilterBots& object) PRINT
    void operator() (td::td_api::tMeUrl& object) PRINT
    void operator() (td::td_api::tMeUrlTypeUser& object) PRINT
    void operator() (td::td_api::tMeUrlTypeSupergroup& object) PRINT
    void operator() (td::td_api::tMeUrlTypeChatInvite& object) PRINT
    void operator() (td::td_api::tMeUrlTypeStickerSet& object) PRINT
    void operator() (td::td_api::tMeUrls& object) PRINT
    void operator() (td::td_api::tdlibParameters& object) PRINT
    void operator() (td::td_api::temporaryPasswordState& object) PRINT
    void operator() (td::td_api::termsOfService& object) PRINT
    void operator() (td::td_api::testBytes& object) PRINT
    void operator() (td::td_api::testInt& object) PRINT
    void operator() (td::td_api::testString& object) PRINT
    void operator() (td::td_api::testVectorInt& object) PRINT
    void operator() (td::td_api::testVectorIntObject& object) PRINT
    void operator() (td::td_api::testVectorString& object) PRINT
    void operator() (td::td_api::testVectorStringObject& object) PRINT
    void operator() (td::td_api::text& object) PRINT
    void operator() (td::td_api::textEntities& object) PRINT
    void operator() (td::td_api::textEntity& object) PRINT
    void operator() (td::td_api::textEntityTypeMention& object) PRINT
    void operator() (td::td_api::textEntityTypeHashtag& object) PRINT
    void operator() (td::td_api::textEntityTypeCashtag& object) PRINT
    void operator() (td::td_api::textEntityTypeBotCommand& object) PRINT
    void operator() (td::td_api::textEntityTypeUrl& object) PRINT
    void operator() (td::td_api::textEntityTypeEmailAddress& object) PRINT
    void operator() (td::td_api::textEntityTypePhoneNumber& object) PRINT
    void operator() (td::td_api::textEntityTypeBankCardNumber& object) PRINT
    void operator() (td::td_api::textEntityTypeBold& object) PRINT
    void operator() (td::td_api::textEntityTypeItalic& object) PRINT
    void operator() (td::td_api::textEntityTypeUnderline& object) PRINT
    void operator() (td::td_api::textEntityTypeStrikethrough& object) PRINT
    void operator() (td::td_api::textEntityTypeCode& object) PRINT
    void operator() (td::td_api::textEntityTypePre& object) PRINT
    void operator() (td::td_api::textEntityTypePreCode& object) PRINT
    void operator() (td::td_api::textEntityTypeTextUrl& object) PRINT
    void operator() (td::td_api::textEntityTypeMentionName& object) PRINT
    void operator() (td::td_api::textEntityTypeMediaTimestamp& object) PRINT
    void operator() (td::td_api::textParseModeMarkdown& object) PRINT
    void operator() (td::td_api::textParseModeHTML& object) PRINT
    void operator() (td::td_api::themeSettings& object) PRINT
    void operator() (td::td_api::thumbnail& object) PRINT
    void operator() (td::td_api::thumbnailFormatJpeg& object) PRINT
    void operator() (td::td_api::thumbnailFormatPng& object) PRINT
    void operator() (td::td_api::thumbnailFormatWebp& object) PRINT
    void operator() (td::td_api::thumbnailFormatGif& object) PRINT
    void operator() (td::td_api::thumbnailFormatTgs& object) PRINT
    void operator() (td::td_api::thumbnailFormatMpeg4& object) PRINT
    void operator() (td::td_api::topChatCategoryUsers& object) PRINT
    void operator() (td::td_api::topChatCategoryBots& object) PRINT
    void operator() (td::td_api::topChatCategoryGroups& object) PRINT
    void operator() (td::td_api::topChatCategoryChannels& object) PRINT
    void operator() (td::td_api::topChatCategoryInlineBots& object) PRINT
    void operator() (td::td_api::topChatCategoryCalls& object) PRINT
    void operator() (td::td_api::topChatCategoryForwardChats& object) PRINT
    void operator() (td::td_api::updateAuthorizationState& object) const {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        td::td_api::downcast_call(*object.authorization_state_, AuthorizationStateHandler(mApplication, mExecutor));
    }
    void operator() (td::td_api::updateNewMessage& object) PRINT
    void operator() (td::td_api::updateMessageSendAcknowledged& object) PRINT
    void operator() (td::td_api::updateMessageSendSucceeded& object) PRINT
    void operator() (td::td_api::updateMessageSendFailed& object) PRINT
    void operator() (td::td_api::updateMessageContent& object) PRINT
    void operator() (td::td_api::updateMessageEdited& object) PRINT
    void operator() (td::td_api::updateMessageIsPinned& object) PRINT
    void operator() (td::td_api::updateMessageInteractionInfo& object) PRINT
    void operator() (td::td_api::updateMessageContentOpened& object) PRINT
    void operator() (td::td_api::updateMessageMentionRead& object) PRINT
    void operator() (td::td_api::updateMessageLiveLocationViewed& object) PRINT
    void operator() (td::td_api::updateNewChat& object) PRINT
    void operator() (td::td_api::updateChatTitle& object) PRINT
    void operator() (td::td_api::updateChatPhoto& object) PRINT
    void operator() (td::td_api::updateChatPermissions& object) PRINT
    void operator() (td::td_api::updateChatLastMessage& object) PRINT
    void operator() (td::td_api::updateChatPosition& object) PRINT
    void operator() (td::td_api::updateChatReadInbox& object) PRINT
    void operator() (td::td_api::updateChatReadOutbox& object) PRINT
    void operator() (td::td_api::updateChatActionBar& object) PRINT
    void operator() (td::td_api::updateChatDraftMessage& object) PRINT
    void operator() (td::td_api::updateChatMessageSender& object) PRINT
    void operator() (td::td_api::updateChatMessageTtl& object) PRINT
    void operator() (td::td_api::updateChatNotificationSettings& object) PRINT
    void operator() (td::td_api::updateChatPendingJoinRequests& object) PRINT
    void operator() (td::td_api::updateChatReplyMarkup& object) PRINT
    void operator() (td::td_api::updateChatTheme& object) PRINT
    void operator() (td::td_api::updateChatUnreadMentionCount& object) PRINT
    void operator() (td::td_api::updateChatVideoChat& object) PRINT
    void operator() (td::td_api::updateChatDefaultDisableNotification& object) PRINT
    void operator() (td::td_api::updateChatHasProtectedContent& object) PRINT
    void operator() (td::td_api::updateChatHasScheduledMessages& object) PRINT
    void operator() (td::td_api::updateChatIsBlocked& object) PRINT
    void operator() (td::td_api::updateChatIsMarkedAsUnread& object) PRINT
    void operator() (td::td_api::updateChatFilters& object) PRINT
    void operator() (td::td_api::updateChatOnlineMemberCount& object) PRINT
    void operator() (td::td_api::updateScopeNotificationSettings& object) PRINT
    void operator() (td::td_api::updateNotification& object) PRINT
    void operator() (td::td_api::updateNotificationGroup& object) PRINT
    void operator() (td::td_api::updateActiveNotifications& object) PRINT
    void operator() (td::td_api::updateHavePendingNotifications& object) PRINT
    void operator() (td::td_api::updateDeleteMessages& object) PRINT
    void operator() (td::td_api::updateChatAction& object) PRINT
    void operator() (td::td_api::updateUserStatus& object) PRINT
    void operator() (td::td_api::updateUser& object) PRINT
    void operator() (td::td_api::updateBasicGroup& object) PRINT
    void operator() (td::td_api::updateSupergroup& object) PRINT
    void operator() (td::td_api::updateSecretChat& object) PRINT
    void operator() (td::td_api::updateUserFullInfo& object) PRINT
    void operator() (td::td_api::updateBasicGroupFullInfo& object) PRINT
    void operator() (td::td_api::updateSupergroupFullInfo& object) PRINT
    void operator() (td::td_api::updateServiceNotification& object) PRINT
    void operator() (td::td_api::updateFile& object) PRINT
    void operator() (td::td_api::updateFileGenerationStart& object) PRINT
    void operator() (td::td_api::updateFileGenerationStop& object) PRINT
    void operator() (td::td_api::updateCall& object) PRINT
    void operator() (td::td_api::updateGroupCall& object) PRINT
    void operator() (td::td_api::updateGroupCallParticipant& object) PRINT
    void operator() (td::td_api::updateNewCallSignalingData& object) PRINT
    void operator() (td::td_api::updateUserPrivacySettingRules& object) PRINT
    void operator() (td::td_api::updateUnreadMessageCount& object) PRINT
    void operator() (td::td_api::updateUnreadChatCount& object) PRINT
    void operator() (td::td_api::updateOption& object) PRINT
    void operator() (td::td_api::updateStickerSet& object) PRINT
    void operator() (td::td_api::updateInstalledStickerSets& object) PRINT
    void operator() (td::td_api::updateTrendingStickerSets& object) PRINT
    void operator() (td::td_api::updateRecentStickers& object) PRINT
    void operator() (td::td_api::updateFavoriteStickers& object) PRINT
    void operator() (td::td_api::updateSavedAnimations& object) PRINT
    void operator() (td::td_api::updateSelectedBackground& object) PRINT
    void operator() (td::td_api::updateChatThemes& object) PRINT
    void operator() (td::td_api::updateLanguagePackStrings& object) PRINT
    void operator() (td::td_api::updateConnectionState& object) PRINT
    void operator() (td::td_api::updateTermsOfService& object) PRINT
    void operator() (td::td_api::updateUsersNearby& object) PRINT
    void operator() (td::td_api::updateDiceEmojis& object) PRINT
    void operator() (td::td_api::updateAnimatedEmojiMessageClicked& object) PRINT
    void operator() (td::td_api::updateAnimationSearchParameters& object) PRINT
    void operator() (td::td_api::updateSuggestedActions& object) PRINT
    void operator() (td::td_api::updateNewInlineQuery& object) PRINT
    void operator() (td::td_api::updateNewChosenInlineResult& object) PRINT
    void operator() (td::td_api::updateNewCallbackQuery& object) PRINT
    void operator() (td::td_api::updateNewInlineCallbackQuery& object) PRINT
    void operator() (td::td_api::updateNewShippingQuery& object) PRINT
    void operator() (td::td_api::updateNewPreCheckoutQuery& object) PRINT
    void operator() (td::td_api::updateNewCustomEvent& object) PRINT
    void operator() (td::td_api::updateNewCustomQuery& object) PRINT
    void operator() (td::td_api::updatePoll& object) PRINT
    void operator() (td::td_api::updatePollAnswer& object) PRINT
    void operator() (td::td_api::updateChatMember& object) PRINT
    void operator() (td::td_api::updateNewChatJoinRequest& object) PRINT
    void operator() (td::td_api::updates& object) PRINT
    void operator() (td::td_api::user& object) PRINT
    void operator() (td::td_api::userFullInfo& object) PRINT
    void operator() (td::td_api::userPrivacySettingShowStatus& object) PRINT
    void operator() (td::td_api::userPrivacySettingShowProfilePhoto& object) PRINT
    void operator() (td::td_api::userPrivacySettingShowLinkInForwardedMessages& object) PRINT
    void operator() (td::td_api::userPrivacySettingShowPhoneNumber& object) PRINT
    void operator() (td::td_api::userPrivacySettingAllowChatInvites& object) PRINT
    void operator() (td::td_api::userPrivacySettingAllowCalls& object) PRINT
    void operator() (td::td_api::userPrivacySettingAllowPeerToPeerCalls& object) PRINT
    void operator() (td::td_api::userPrivacySettingAllowFindingByPhoneNumber& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleAllowAll& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleAllowContacts& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleAllowUsers& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleAllowChatMembers& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleRestrictAll& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleRestrictContacts& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleRestrictUsers& object) PRINT
    void operator() (td::td_api::userPrivacySettingRuleRestrictChatMembers& object) PRINT
    void operator() (td::td_api::userPrivacySettingRules& object) PRINT
    void operator() (td::td_api::userStatusEmpty& object) PRINT
    void operator() (td::td_api::userStatusOnline& object) PRINT
    void operator() (td::td_api::userStatusOffline& object) PRINT
    void operator() (td::td_api::userStatusRecently& object) PRINT
    void operator() (td::td_api::userStatusLastWeek& object) PRINT
    void operator() (td::td_api::userStatusLastMonth& object) PRINT
    void operator() (td::td_api::userTypeRegular& object) PRINT
    void operator() (td::td_api::userTypeDeleted& object) PRINT
    void operator() (td::td_api::userTypeBot& object) PRINT
    void operator() (td::td_api::userTypeUnknown& object) PRINT
    void operator() (td::td_api::users& object) PRINT
    void operator() (td::td_api::validatedOrderInfo& object) PRINT
    void operator() (td::td_api::vectorPathCommandLine& object) PRINT
    void operator() (td::td_api::vectorPathCommandCubicBezierCurve& object) PRINT
    void operator() (td::td_api::venue& object) PRINT
    void operator() (td::td_api::video& object) PRINT
    void operator() (td::td_api::videoChat& object) PRINT
    void operator() (td::td_api::videoNote& object) PRINT
    void operator() (td::td_api::voiceNote& object) PRINT
    void operator() (td::td_api::webPage& object) PRINT
    void operator() (td::td_api::webPageInstantView& object) PRINT

    std::shared_ptr<QueuedExecutor> mExecutor;
    wxApp* mApplication;
};

class ReceiveAndHandleResponseTask final: public ITask {
public:
    ReceiveAndHandleResponseTask(const std::shared_ptr<td::ClientManager> clientManager,
                                 std::unique_ptr<ResponseHandler>&& responseHandler):
        mClientManager(clientManager), mHandler(std::move(responseHandler)) {}
    TaskExecutionResult execute() override {
        auto response = clientManager->receive(0);
        if(response.object) {
            std::cout << "response request id: " << response.request_id << std::endl;
            td::td_api::downcast_call(*response.object, *mHandler);
        }
        return TaskExecutionResult::Resubmit;
    }
    std::shared_ptr<td::ClientManager> mClientManager;
    std::unique_ptr<ResponseHandler> mHandler;
};

void io_loop(wxApp* application) {
    std::cout << "hello" << std::endl;
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));
    clientManager = std::make_shared<td::ClientManager>();
    clientId = clientManager->create_client_id();

    std::cout << "client id: " << clientId << std::endl;
    
    requestId++;
    std::cout << "sending request: " << requestId << std::endl;
    clientManager->send(clientId, requestId, td::td_api::make_object<td::td_api::getOption>("version"));

    auto executor = std::make_shared<QueuedExecutor>();
    executor->submit(std::make_shared<ReceiveAndHandleResponseTask>(clientManager, std::make_unique<ResponseHandler>(application, executor)));
    while(true) {
        std::this_thread::yield();
    }
}
