# Version 1.0.0 – Initial Public Release

### Overview
This is the first official release of **Roblox Studio Discord Presence**, a lightweight integration that displays your Roblox Studio activity on your Discord profile using Discord Rich Presence.

The system is split into two components:
- A **Roblox Studio Plugin** that gathers session activity.
- A **C++ Desktop Application** that relays data to Discord.

---

### Features

- **Live Activity Tracking**
  - Displays the current script being edited.
  - Shows the name of the place (published or local).
  - Auto-updates in real-time as you switch tabs or go idle.
  - Uses distinct icons for Script, LocalScript, and ModuleScript.

- **Discord Integration**
  - Fully compatible with Discord Rich Presence API.
  - Automatically shows duration since session activation.
  - Updates every second for maximum responsiveness.

- **Idle Mode**
  - Automatically enters an idle state when no script is open.

---

### Installation

#### 1. Install the Plugin
- Download the `.rbxm` plugin file from the [Releases](#) section.
- In Roblox Studio: `Plugins → Manage Plugins → Import` the file.
- Click the "Activate Presence" button from the **Discord Presence** toolbar.

#### 2. Run the Desktop App
- Download and run the `RobloxPresenceApp.exe` (or compile from source).
- The app starts a local server at `http://localhost:3000/updateRobloxPresence`.

---

### Developer Notes

- The plugin uses `HttpService` to send POST requests to the local server.
- The app is built using `crow_all.h` (lightweight HTTP server) and `Discord Social SDK`.
- Ensure `localhost` communication is unblocked in your firewall or antivirus.

---

### Permissions

- This plugin only uses `HttpService`, `ScriptEditorService`, `AssetService`, and `RunService`.
- No external tracking or analytics are used.
- All data is processed locally and never leaves your device.

---

### Future Plans (v1.1+)

- Multiplayer (Team Create) awareness.
- Custom status message editing.
- Ability to run as start-up application.

---

### 📣 Credits

- **Developer:** Dave (@Bagel_Seedz)
- **Special Thanks:** Open source community (Crow, Discord SDK)
- **Icon Sources:** Roblox Studio Icons (For Fair Use within Roblox context)

---

