# DeathHeatmap 📊

A Geometry Dash mod built with [Geode](https://geode-sdk.org/) that visualizes your death locations using an interactive heatmap.

![Mod Screenshot](logo.png) ## 🌟 Features

* **Real-time Tracking:** Tracks your death percentage every attempt.
* **Heatmap Visualization:** A visual bar accessible from the Level Info menu showing where you struggle the most.
* **Gradient Visuals:**
    * 🟦 **Cyan:** Low death count (Safe).
    * 🟥 **Red:** High death count (Danger Zone).
* **Interactive UI:** Hover your mouse over any part of the bar to see:
    * Exact Percentage.
    * Total Death Count at that spot.
* **Session Based:** Currently tracks deaths for the active game session.

## 🚀 How to Use

1.  **Play a Level:** Just play normally! The mod runs in the background.
2.  **Die a few times:** (Ideally, don't, but it happens).
3.  **Check the Heatmap:**
    * Exit the level.
    * Click the **Graph Button** on the left side of the Level Info menu.
    * Hover over the colored bar to analyze your run.

## 🛠️ Installation

> **Note:** This mod is currently **not available on the Geode Index**. You must install it manually.

1.  Download and install [Geode](https://geode-sdk.org/).
2.  Go to the **[Releases](../../releases)** tab on this repository.
3.  Download the latest `.geode` file (e.g., `irham.deathheatmap.geode`).
4.  Open Geometry Dash.
5.  Click the Geode button on the main menu -> Click the **Folder Icon** (Open Mods Folder).
6.  Drag and drop the downloaded `.geode` file into that folder.
7.  Restart the game.

## 📅 Upcoming Features

* [ ] **Persistent Data Streaming:** Implement real-time auto-saving to prevent data loss on crashes and allow long-term tracking.
* [ ] **Smart Choke Point Detector:** Analyze death data to automatically identify and list your top 3 "Choke Points" (percentages where you die the most).
* [ ] **Mod Settings:** Options to customize bar colors, opacity, and toggle visibility.

## Credits

* **Irham** - Developer
* Built with the Geode SDK.

---
*Found a bug? Open an issue in the repository!*