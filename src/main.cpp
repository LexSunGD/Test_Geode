#include <Geode/Geode.hpp>
#include <Geode/modify/CommentCell.hpp>

using namespace geode::prelude;

class $modify(MyCommentCell, CommentCell) {
    
    // loadFromComment es el método donde se construye visualmente la celda del comentario
    void loadFromComment(GJComment* comment) {
        // Ejecutamos la función original primero para que se cree el comentario base
        CommentCell::loadFromComment(comment);

        // Validamos si la configuración del mod está activa
        bool enabled = Mod::get()->getSettingValue<bool>("show-like");
        if (!enabled) return;

        // Buscamos el menú principal de botones dentro de la celda (usualmente se llama "main-menu")
        if (auto menu = this->getChildByID("main-menu")) {
            
            // Creamos el sprite del botón (puedes usar "GJ_likeBtn_001.png" o "GJ_dislikeBtn_001.png")
            auto btnSprite = CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png");
            
            if (btnSprite) {
                // Hacemos el botón un poco más pequeño para que encaje bien en la celda
                btnSprite->setScale(0.7f);

                auto myButton = CCMenuItemSpriteExtra::create(
                    btnSprite,
                    this,
                    menu_selector(MyCommentCell::onShortcutCommentLike)
                );

                // Guardamos el puntero del comentario dentro del botón usando setUserObject 
                // para poder leerlo cuando hagamos clic.
                myButton->setUserObject(comment);
                myButton->setID("shortcut-comment-like-button");

                // Añadimos el botón al menú de la celda
                menu->addChild(myButton);
                menu->updateLayout();
            }
        }
    }

    // Función que se ejecuta al presionar nuestro nuevo botón
    void onShortcutCommentLike(CCObject* sender) {
        auto button = static_cast<CCMenuItemSpriteExtra*>(sender);
        if (!button) return;

        // Recuperamos el comentario asociado a este botón específico
        auto comment = static_cast<GJComment*>(button->getUserObject());
        if (!comment) return;

        if (GameLevelManager::sharedState()) {
            // Identificamos el tipo de comentario:
            // Si comment->m_accountID es mayor a 0, suele ser un post de cuenta (tipo 3). 
            // Si no, es un comentario de nivel (tipo 2).
            int itemType = (comment->m_accountID > 0) ? 3 : 2;
            int commentID = comment->m_commentID;

            // Enviamos el Like directo al servidor a través del manager del juego
            // Parámetros: (TipoItem, ID, EsLike?, Especial/Contexto)
            GameLevelManager::sharedState()->likeItem(
                static_cast<LikeItemType>(itemType), 
                commentID, 
                true, // true = Like, false = Dislike
                0
            );

            // Mostramos una alerta de confirmación en pantalla
            FLAlertLayer::create("Like", "¡Like enviado al comentario!", "OK")->show();
        }
    }
};
