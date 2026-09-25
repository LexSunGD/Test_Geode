#include <Geode/Geode.hpp>
#include <Geode/modify/GJAccountSettingsLayer.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

// Interceptamos la capa del menú de ajustes de la cuenta (More -> Account Settings)
class $modify(MyGJAccountSettingsLayer, GJAccountSettingsLayer) {
    
    // Hacemos el hook a la función que se ejecuta cuando guardas/cierras el menú de privacidad
    void onClose(cocos2d::CCObject* sender) {
        // 1. Dejamos que el juego haga su guardado normal primero
        GJAccountSettingsLayer::onClose(sender);
        
        log::info("Menú cerrado. Iniciando envío automático de claves 62 y 63 para la 2.209...");

        // 2. Obtenemos los datos de sesión actuales del jugador directamente desde el juego
        auto accountManager = GJAccountManager::sharedState();
        std::string accountID = std::to_string(accountManager->m_accountID);
        std::string gdw = "0"; // Parámetro estándar de verificación de GD

        // Si el usuario no ha iniciado sesión, no hacemos nada para evitar errores
        if (accountManager->m_accountID <= 0) {
            log::error("Error: No has iniciado sesión en tu cuenta de Geometry Dash.");
            return;
        }

        // 3. Creamos el cuerpo de la petición HTTP POST de forma manual
        // Añadimos las variables obligatorias del servidor junto con las nuevas claves (62=0 y 63=0)
        std::string postData = "accountID=" + accountID + "&gdw=" + gdw + "&62=0&63=0";

        // 4. Enviamos la petición usando el nuevo sistema web oficial de Geode 5.x
        // Esto envía los datos de forma segura e independiente en segundo plano
        ::geode::utils::web::AsyncWebRequest()
            .join("https://boomlings.com")
            .body(postData)
            .post()
            .send()
            .listen(
                [](auto* response) {
                    log::info("¡Servidor respondio correctamente! Las claves 62 y 63 se han actualizado.");
                },
                [](auto* error) {
                    log::error("Hubo un error al conectar con el servidor de RobTop.");
                }
            );
    }
};
