$(document).ready(function() {
    $(".msg_wrap")
    .css('opacity', 0)
    .slideDown(500)
    .animate(
        { opacity: 1 },
        { queue: false, duration: 500 }
    );
});

function hide(element) {
    $(element).fadeOut(350).slideUp(350).dequeue();
}