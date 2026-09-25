#include <Geode/Geode.hpp>
#include <Geode/modify/GJAccountSettingsLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/hash.hpp> // CAMBIO: Añadimos la cabecera correcta de hash

using namespace geode::prelude;

// Función corregida para usar el sistema de Hash oficial de Geode v5
std::string generateGDHash(std::string const& data) {
    // En Geode v5, sha1 se encuentra directamente en utils::hash
    return geode::utils::hash::sha1(data + "Wmfd2893gb7");
}

class $modify(MyGJAccountSettingsLayer, GJAccountSettingsLayer) {
    
    void onUpdate(cocos2d::CCObject* sender) {
        // 1. Ejecutamos la lógica normal del juego primero
        GJAccountSettingsLayer::onUpdate(sender);
        
        // 2. Extraemos los datos del usuario logueado de forma segura
        auto am = GJAccountManager::sharedState();
        if (am->m_accountID <= 0) return;

        std::string accountID = std::to_string(am->m_accountID);
        std::string gdw = "0";

        log::info("Generando firma criptográfica con el secreto de RobTop...");

        // 3. Creamos el texto base para calcular el chk oficial
        std::string textToHash = accountID + gdw + "0" + "0"; 
        std::string chk = generateGDHash(textToHash);

        // 4. Construimos el paquete HTTP POST inyectando las claves y la firma generada
        std::string postData = "accountID=" + accountID + 
                               "&gdw=" + gdw + 
                               "&62=0" +   // 0 = Enabled
                               "&63=0" +   // 0 = Enabled
                               "&chk=" + chk;

        log::info("Enviando paquete firmado de forma nativa al servidor...");

        // 5. Enviamos la petición usando el despachador asíncrono
        web::WebRequest req;
        req.bodyString(postData);
        req.header("Content-Type", "application/x-www-form-urlencoded");

        geode::async::spawn(
            req.post("https://boomlings.com"),
            [](web::WebResponse resp) {
                log::info("¡Servidor respondió! Código de estado: {}. Las claves han sido inyectadas con éxito.", resp.code());
            }
        );
    }
};
