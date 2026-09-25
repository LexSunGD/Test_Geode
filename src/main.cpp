#include <Geode/Geode.hpp>
#include <Geode/modify/WebRequestBuilder.hpp>

using namespace geode::prelude;

// Interceptamos la clase constructora de peticiones web de Geometry Dash
class $modify(MyWebRequestBuilder, WebRequestBuilder) {
    
    // Hacemos el hook a la función que añade el cuerpo (body) a la petición HTTP POST
    WebRequest& bodyString(std::string const& body) {
        std::string modifiedBody = body;

        // Comprobamos si la petición actual va dirigida a la API de actualizar la cuenta
        // Nota: RobTop suele usar el endpoint 'updateGJAccSettings.php' para esto
        if (body.find("accountID=") != std::string::npos && this->getURL().find("updateGJAccSettings") != std::string::npos) {
            
            // Añadimos las claves 62 y 63 con valor 0 (que corresponde a Habilitado / Todo el mundo)
            // El formato debe respetar la estructura de parámetros HTTP (&clave=valor)
            modifiedBody += "&62=0&63=0";
            
            log::info("¡Modificación exitosa! Se han inyectado las claves 62 y 63 con valor 'Enabled'.");
        }

        // Llamamos a la función original del juego pasando el texto ya modificado
        return WebRequestBuilder::bodyString(modifiedBody);
    }
};
