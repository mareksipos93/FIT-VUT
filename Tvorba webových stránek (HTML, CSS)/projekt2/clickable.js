$(document).ready(function () {
    /* On load show/hide box depending on cookie */
    if ($.cookie('click_cookie') === "true") {
        $.cookie('click_cookie', "true");
        $("#clickable p").show();
    } else {
        $.cookie('click_cookie', "false");
        $("#clickable p").hide();
    }

    /* When clicked */
    $("#clickable h3").click(function () {
        /* Animate box */
        $("#clickable p").animate({
            height: "toggle",
            opacity: "toggle"
        });
        /* Toggle cookie */
        if ($.cookie('click_cookie') === "true") {
            $.cookie('click_cookie', "false");
        } else {
            $.cookie('click_cookie', "true");
        }
    });
});