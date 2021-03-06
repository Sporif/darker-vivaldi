// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_IMPORTER_EXTERNAL_PROCESS_IMPORTER_CLIENT_H_
#define CHROME_BROWSER_IMPORTER_EXTERNAL_PROCESS_IMPORTER_CLIENT_H_

#include <stddef.h>
#include <stdint.h>

#include <string>
#include <vector>

#include "base/compiler_specific.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/strings/string16.h"
#include "build/build_config.h"
#include "chrome/common/importer/importer_autofill_form_data_entry.h"
#include "chrome/common/importer/importer_data_types.h"
#include "chrome/common/importer/importer_url_row.h"
#include "components/favicon_base/favicon_usage_data.h"
#include "components/history/core/browser/history_types.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/utility_process_host_client.h"

class ExternalProcessImporterHost;
struct ImportedBookmarkEntry;
struct ImportedNotesEntry;
struct ImportedSpeedDialEntry;
struct ImportedFaviconUsage;
class InProcessImporterBridge;

namespace autofill {
struct PasswordForm;
}

namespace content{
class UtilityProcessHost;
}

namespace importer {
#if defined(OS_WIN)
struct ImporterIE7PasswordInfo;
#endif
struct ImporterAutofillFormDataEntry;
struct SearchEngineInfo;
}

// This class is the client for the out of process profile importing.  It
// collects notifications from this process host and feeds data back to the
// importer host, who actually does the writing.
class ExternalProcessImporterClient : public content::UtilityProcessHostClient {
 public:
  ExternalProcessImporterClient(
      base::WeakPtr<ExternalProcessImporterHost> importer_host,
      const importer::SourceProfile& source_profile,
      const importer::ImportConfig &import_config,
      InProcessImporterBridge* bridge);

  // Launches the task to start the external process.
  void Start();

  // Called by the ExternalProcessImporterHost on import cancel.
  void Cancel();

  // UtilityProcessHostClient implementation:
  void OnProcessCrashed(int exit_code) override;
  bool OnMessageReceived(const IPC::Message& message) override;

  // Message handlers
  void OnImportStart();
  void OnImportFinished(bool succeeded, const std::string& error_msg);
  void OnImportItemStart(int item);
  void OnImportItemFinished(int item);
  void OnImportItemFailed(int item, const std::string& error_msg);
  void OnHistoryImportStart(size_t total_history_rows_count);
  void OnHistoryImportGroup(
      const std::vector<ImporterURLRow>& history_rows_group,
      int visit_source);
  void OnHomePageImportReady(const GURL& home_page);
  void OnBookmarksImportStart(const base::string16& first_folder_name,
                              size_t total_bookmarks_count);
  void OnBookmarksImportGroup(
      const std::vector<ImportedBookmarkEntry>& bookmarks_group);
  void OnNotesImportStart(const base::string16& first_folder_name,
                          size_t total_notes_count);
  void OnNotesImportGroup(
      const std::vector<ImportedNotesEntry>& notes_group);
  void OnSpeedDialImportStart(size_t total_count);
  void OnSpeedDialImportGroup(const std::vector<ImportedSpeedDialEntry>& group);
  void OnFaviconsImportStart(size_t total_favicons_count);
  void OnFaviconsImportGroup(
      const favicon_base::FaviconUsageDataList& favicons_group);
  void OnPasswordFormImportReady(const autofill::PasswordForm& form);
  void OnKeywordsImportReady(
      const std::vector<importer::SearchEngineInfo>& search_engines,
      bool unique_on_host_and_path);
  void OnFirefoxSearchEngineDataReceived(
      const std::vector<std::string> search_engine_data);
  void OnAutofillFormDataImportStart(
      size_t total_autofill_form_data_entry_count);
  void OnAutofillFormDataImportGroup(const std::vector<
      ImporterAutofillFormDataEntry>& autofill_form_data_entry_group);
#if defined(OS_WIN)
  void OnIE7PasswordReceived(
        const importer::ImporterIE7PasswordInfo& importer_password_info);
#endif

 protected:
  ~ExternalProcessImporterClient() override;

 private:
  // Notifies the importerhost that import has finished, and calls Release().
  void Cleanup();

  // Cancel import process on IO thread.
  void CancelImportProcessOnIOThread();

  // Report item completely downloaded on IO thread.
  void NotifyItemFinishedOnIOThread(importer::ImportItem import_item);

  // Report item import failed on IO thread
  void NotifyItemFailedOnIOThread(importer::ImportItem import_item,
                                  const std::string& error);

  // Creates a new UtilityProcessHost, which launches the import process.
  void StartProcessOnIOThread(content::BrowserThread::ID thread_id);

  // These variables store data being collected from the importer until the
  // entire group has been collected and is ready to be written to the profile.
  std::vector<ImporterURLRow> history_rows_;
  std::vector<ImportedBookmarkEntry> bookmarks_;
  favicon_base::FaviconUsageDataList favicons_;
  std::vector<ImportedNotesEntry> notes_;
  std::vector<ImporterAutofillFormDataEntry> autofill_form_data_;

  std::vector<ImportedSpeedDialEntry> speeddial_;

  // Usually some variation on IDS_BOOKMARK_GROUP_...; the name of the folder
  // under which imported bookmarks will be placed.
  base::string16 bookmarks_first_folder_name_;

  // Total number of bookmarks to import.
  size_t total_bookmarks_count_;

   // Usually some variation on IDS_NOTES_GROUP_...; the name of the folder
  // under which imported notes will be placed.
  base::string16 notes_first_folder_name_;

  // Total number of notes to import.
  size_t total_notes_count_;

  // Total number of speed dial to import.
  size_t total_speeddial_count_;

  // Total number of history items to import.
  size_t total_history_rows_count_;

  // Total number of favicons to import.
  size_t total_favicons_count_;

  // Total number of autofill form data entries to import.
  size_t total_autofill_form_data_entry_count_;

  // Notifications received from the ProfileImportProcessHost are passed back
  // to process_importer_host_, which calls the ProfileWriter to record the
  // import data.  When the import process is done, process_importer_host_
  // deletes itself. This is a weak ptr so that any messages received after
  // the host has deleted itself are ignored (e.g., it's possible to receive
  // OnProcessCrashed() after NotifyImportEnded()).
  base::WeakPtr<ExternalProcessImporterHost> process_importer_host_;

  // Handles sending messages to the external process.  Deletes itself when
  // the external process dies (see
  // BrowserChildProcessHost::OnChildDisconnected).
  base::WeakPtr<content::UtilityProcessHost> utility_process_host_;

  // Data to be passed from the importer host to the external importer.
  importer::SourceProfile source_profile_;
  const importer::ImportConfig &import_config_;

  // Takes import data coming over IPC and delivers it to be written by the
  // ProfileWriter.
  scoped_refptr<InProcessImporterBridge> bridge_;

  // True if import process has been cancelled.
  bool cancelled_;

  DISALLOW_COPY_AND_ASSIGN(ExternalProcessImporterClient);
};

#endif  // CHROME_BROWSER_IMPORTER_EXTERNAL_PROCESS_IMPORTER_CLIENT_H_
