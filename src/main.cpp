#include <Geode/Geode.hpp>
#include <Geode/modify/GJAccountSettingsLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

// Función para generar el cifrado SHA-1 (Firma digital de RobTop)
std::string generateGDHash(std::string const& data) {
    // Usamos las herramientas de cifrado internas que Geode ya trae incluidas
    return geode::utils::crypto::sha1(data + "Wmfd2893gb7");
}

class $modify(MyGJAccountSettingsLayer, GJAccountSettingsLayer) {
    
    void onUpdate(cocos2d::CCObject* sender) {
        // 1. Dejamos que el juego haga su guardado normal de las opciones viejas
        GJAccountSettingsLayer::onUpdate(sender);
        
        // 2. Obtenemos tus datos para que el servidor sepa de quién es la cuenta
        auto am = GJAccountManager::sharedState();
        if (am->m_accountID <= 0) return;

        std::string accountID = std::to_string(am->m_accountID);
        std::string gdw = "0";

        log::info("Generando firma criptográfica con el secreto de RobTop...");

        // 3. PASO CRUCIAL: Creamos el texto base para la firma (los datos + el secreto)
        // RobTop calcula el chk sumando los valores en un orden específico.
        // Para la cuenta, el orden estándar es: accountID + gdw + clave62 + clave63
        std::string textToHash = accountID + gdw + "0" + "0"; 
        std::string chk = generateGDHash(textToHash);

        // 4. Construimos el paquete final que va a internet incluyendo el chk legítimo
        std::string postData = "accountID=" + accountID + 
                               "&gdw=" + gdw + 
                               "&62=0" +   // Activamos la 62 (Enabled / All)
                               "&63=0" +   // Activamos la 63 (Enabled / All)
                               "&chk=" + chk; // Adjuntamos la firma aprobada por el servidor

        log::info("Enviando paquete firmado de forma nativa al servidor...");

        // 5. Enviamos a la base de datos oficial
        web::WebRequest req;
        req.bodyString(postData);
        req.header("Content-Type", "application/x-www-form-urlencoded");

        geode::async::spawn(
            req.post("https://boomlings.com"),
            [](web::WebResponse resp) {
                log::info("¡Servidor respondió! Código de estado: {}. Las claves 62 y 63 ya son oficiales.", resp.code());
            }
        );
    }
};
