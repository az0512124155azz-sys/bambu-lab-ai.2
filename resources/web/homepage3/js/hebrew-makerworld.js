// Bambu Studio AI — Hebrew MakerWorld search helpers.
window.addEventListener('load', function () {
    window.setTimeout(function () {
        var input = document.getElementById('HotModel_Search_Input');
        if (input) {
            input.placeholder = 'Search MakerWorld — חיפוש בעברית או באנגלית';
            input.setAttribute('dir', 'auto');
        }
    }, 0);
});

function OpenSynagogueToys()
{
    var hebrewQuery = 'צעצועים לבית כנסת שקטים קטנים בטוחים לילדים ללא חלקים קטנים';
    var input = document.getElementById('HotModel_Search_Input');
    if (input) input.value = hebrewQuery;
    var message = {
        sequence_id: Math.round(new Date() / 1000),
        command: 'homepage_online_search',
        keyword: hebrewQuery
    };
    SendWXMessage(JSON.stringify(message));
}
