#include <geode/prelude.hpp>
#include <geode/utils/web.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

class $modify(MyPlayLayer, PlayLayer) {
    
    bool init(GJGameLevel* level, bool useReplay, bool dontSave) {
        if (!PlayLayer::init(level, useReplay, dontSave)) {
            return false;
        }

        // 1. Creamos una etiqueta de texto temporal que diga "Cargando..."
        // Usamos la fuente estándar del juego (bigFont.fnt)
        auto etiquetaOnline = CCLabelBMFont::create("Cargando texto...", "bigFont.fnt");
        
        // Le asignamos un ID único para poder encontrarla e identificarla más tarde
        etiquetaOnline->setID("texto-remoto-github"_spr);
        
        // La hacemos un poco más pequeña (escala 0.4) para que no estorbe
        etiquetaOnline->setScale(0.4f);
        
        // La posicionamos en la esquina inferior izquierda de la pantalla
        etiquetaOnline->setPosition({ 10.0f, 10.0f });
        etiquetaOnline->setAnchorPoint({ 0.0f, 0.0f }); // Anclaje a la izquierda
        
        // La añadimos visualmente a la capa del nivel (PlayLayer)
        this->addChild(etiquetaOnline, 100);

        // 2. CORREGIDO: Aquí está ahora SÍ la dirección completa y exacta de tu repositorio y archivo
        std::string url = "https://githubusercontent.com/Test_geode_online_text/main/datos.json" + std::to_string(std::time(nullptr));

        // 3. Realizamos la petición de red en segundo plano
        web::WebRequest()
            .get(url)
            .listen([this](web::WebResponse* response) {
                if (response->isSuccess()) {
                    auto jsonResult = response->json();
                    if (jsonResult.isSuccess()) {
                        auto data = jsonResult.value();
                        
                        // Extraemos el string de tu JSON
                        std::string textoRemoto = data["texto_pantalla"].asString().value_or("Llave no encontrada");

                        // Volvemos al hilo principal para actualizar la interfaz del juego
                        Loader::get()->queueInMainThread([this, textoRemoto]() {
                            // Buscamos la etiqueta que creamos antes usando su ID único
                            if (auto etiqueta = this->getChildByID("texto-remoto-github"_spr)) {
                                // Cast seguro a CCLabelBMFont para poder cambiarle el texto
                                if (auto label = typeinfo_cast<CCLabelBMFont*>(etiqueta)) {
                                    label->setString(textoRemoto.c_str());
                                }
                            }
                        });
                    }
                } else {
                    // Si falla el internet, actualizamos la etiqueta con el error
                    Loader::get()->queueInMainThread([this]() {
                        if (auto etiqueta = this->getChildByID("texto-remoto-github"_spr)) {
                            if (auto label = typeinfo_cast<CCLabelBMFont*>(etiqueta)) {
                                label->setString("Error de conexión");
                            }
                        }
                    });
                }
            });

        return true;
    }
};
