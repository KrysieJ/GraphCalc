// SPDX-FileCopyrightText: 2026 Krystal Jasmine
// SPDX-License-Identifier: LicenseRef-NewCode-TBD

//! Application window. Phase 0a placeholder: an empty window, no logic.

// Named here only so the licence and advisory checks cover these crates
// from Phase 0a onwards; nothing uses them yet.
use egui_plot as _;
use gc_engine as _;
use gc_script as _;
use gc_settings as _;

struct PlaceholderApp;

impl eframe::App for PlaceholderApp {
    fn ui(&mut self, _ui: &mut eframe::egui::Ui, _frame: &mut eframe::Frame) {}
}

fn main() -> eframe::Result {
    eframe::run_native(
        "Placeholder",
        eframe::NativeOptions::default(),
        Box::new(|_cc| Ok(Box::new(PlaceholderApp))),
    )
}
