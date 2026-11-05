# 003 Security hardening

Prevent untrusted station-directory data from causing cora to access local or
otherwise non-public network services.

## Scope

- Validate station stream URLs before persisting them and before playback.
- Support only explicitly approved stream schemes (`https` and, if retained
  for compatibility, `http`).
- Reject URLs with credentials, non-web schemes, and destinations that are
  loopback, private, link-local, multicast, unspecified, or otherwise
  non-public.
- Apply the same destination policy after DNS resolution and redirects.
- Display a concise recoverable error when a selected station's URL is
  rejected.
- Add unit tests covering accepted public HTTP(S) URLs and every rejected URL
  or destination class.

## Acceptance criteria

- A station URL using any scheme other than an approved HTTP(S) scheme is not
  played.
- URLs containing credentials, malformed hosts, or local/private/link-local
  addresses are rejected.
- A hostname resolving to a prohibited address is rejected.
- Redirects cannot bypass destination validation.
- Valid public stream URLs continue to play.
- Existing CLI and TUI behavior remains unchanged for valid streams.

## Open topics

- Confirm whether plain `http` streams must remain supported or whether the
  application can require `https`.
- Choose the component that enforces validation for libVLC redirects, since
  validation before `libvlc_media_new_location` alone cannot inspect later
  redirect targets.
- Define DNS rebinding handling and the resolver behavior to use on each
  supported platform.
- Decide whether station records with invalid URLs are skipped during import,
  retained but marked unusable, or replaced when a valid alternate URL exists.
