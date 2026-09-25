#include <Geode/Geode.hpp>
#include <Geode/modify/GJAccountSettingsLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

// Interceptamos el menú de configuración de cuenta
class $modify(MyGJAccountSettingsLayer, GJAccountSettingsLayer) {
    
    // CAMBIO CLAVE: Ahora interceptamos la función que se ejecuta al presionar "Update"
    void onUpdate(cocos2d::CCObject* sender) {
        // 1. Ejecutamos primero la lógica normal del juego para que procese tus cambios visuales
        GJAccountSettingsLayer::onUpdate(sender);
        
        // 2. Extraemos los datos del usuario logueado de forma segura
        auto accountManager = GJAccountManager::sharedState();
        if (accountManager->m_accountID <= 0) {
            log::error("Error: No has iniciado sesión en Geometry Dash.");
            return;
        }

        std::string accountID = std::to_string(accountManager->m_accountID);
        // Construimos el cuerpo de la petición estándar (Form URL Encoded)
        std::string postData = "accountID=" + accountID + "&gdw=0&62=0&63=0";

        log::info("¡Botón Update presionado! Enviando petición asíncrona para claves 62 y 63...");

        // 3. Configuramos la petición web nativa de Geode
        web::WebRequest req;
        req.bodyString(postData);
        req.header("Content-Type", "application/x-www-form-urlencoded");

        // 4. Despachamos la solicitud en segundo plano usando el sistema async de Geode v5
        geode::async::spawn(
            req.post("https://boomlings.com"),
            [](web::WebResponse resp) {
                log::info("¡Petición enviada! Código de respuesta del servidor: {}", resp.code());
            }
        );
    }
};
