# Bambu Studio AI extension

This branch preserves upstream Bambu Studio and adds an opt-in AI tool window.
Open it from **Preferences → AI Assistant** or press **Ctrl+Shift+Z**.

## Providers

- NVIDIA uses the OpenAI-compatible endpoint `https://integrate.api.nvidia.com/v1`.
- The API key is held in memory for the current process and is never committed.
- Ollama is probed at `http://127.0.0.1:11434/api/tags`; it is selected only when that health check succeeds.

## Safety model

AI output is untrusted. `AIToolGuard` is deny-by-default. Read-only operations may run without approval; slicer and printer mutations require an explicit approval. Unknown tool names never run. Remote sharing is also deny-by-default and must be backed by an authenticated relay before it grants access.

## Attachments

The UI accepts any file selected by the user. The provider receives only filenames in the current implementation. Binary upload, MIME validation, size limits and redaction must be implemented before attachments are sent outside the computer.

## Email and calendar

The settings page stores connector names and a notification address, but never OAuth client secrets. Gmail/Google Calendar or Outlook access must use OAuth through a separately authorized MCP relay. The app does not embed credentials. A production relay should expose only these scoped tools:

- `send_print_alert(recipient, summary, image)`
- `create_print_complete_event(calendar, title, finished_at)`

The connector is not considered active until it completes an authenticated health check.

## Current implementation status

The panel, provider transport, local Ollama discovery, attachments picker, session-only NVIDIA key, settings persistence, sharing-rule UI and safety policy are implemented. Direct printer control, authenticated remote sharing, camera anomaly detection and MCP email/calendar execution remain adapters that must be implemented and tested against supported printer firmware before release.
