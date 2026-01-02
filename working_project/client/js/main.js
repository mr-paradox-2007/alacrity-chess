document.addEventListener('DOMContentLoaded', () => {
    if (auth.is_authenticated()) {
        ui.show_dashboard();
    } else {
        ui.show_login_page();
    }
});
