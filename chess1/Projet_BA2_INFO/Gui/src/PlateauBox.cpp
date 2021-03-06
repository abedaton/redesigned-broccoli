#ifndef PLATEAUBOX_CPP
#define PLATEAUBOX_CPP
#include "PlateauBox.hpp"


PlateauBox::PlateauBox(int x, int y, int sideLenght = 65, QGraphicsItem *parent) : QGraphicsRectItem(parent), _sideLenght(sideLenght) {
    setRect(0, 0, sideLenght, sideLenght);
    setPos(x,y);
    // setPen(_pen);
    _brush.setStyle(Qt::SolidPattern);
    setBrush(_brush);
    _piece = nullptr;
    _fog = nullptr;
    if(_sideLenght >= 50)
        _plateauSize = 8;
    else 
        _plateauSize = 24;
}

void PlateauBox::setColor(QColor color) {
    _brush.setColor(color);
    setBrush(color);
}

void PlateauBox::setFirstColor(QColor color) {
    _color = color;
    setColor(_color);
}

void PlateauBox::setPosition(int x, int y) {
    _pos = std::pair<int, int>(x, y);
}

void PlateauBox::mousePressEvent(QGraphicsSceneMouseEvent* event) {
	
	//(void)event;
	
    // std::cout << _pos.first() << "," << _pos.second() << std::endl;
    //setColor(Qt::red);
    //std::cout << _color.rgb() << std::endl;
    // if(_piece != nullptr && (_brush != Qt::red && _brush != Qt::green)){
    //     std::cout << "wsh ya une piece" << std::endl;
    //     std::cout << "PRINT PATH " << _piece->get_common_path() << std::endl;
    //     _scene->resetAllColors();

    //     //_scene->showMoves(moves,cap);
    //     //_scene->setPriorityBox(this);
    // }

    // else if(_brush.color() == Qt::red && _piece == nullptr){

    //     //movePiece(_scene->getPriorityBox());
    //     //std::cout << "FINAL : " << _piece << std::endl;

    // }
    // else if(_brush.color() == Qt::green && _piece != nullptr){
    //     delete _piece;
    //     //movePiece(_scene->getPriorityBox());
     

    // }

    // else{
    //     _scene->resetAllColors();
    //     std::cout << "oupsi pas de piece" << std::endl;
    // }

    
    _scene->resetAllColors();

    std::string numer = std::to_string(_plateauSize - std::get<0>(_pos));
    char letter = 65 + std::get<1>(_pos);
    std::string output = letter+numer;

    std::cout << output << std::endl; ///CAN

    sendPosition(output);




}


void PlateauBox::setPiece(ChessItem *piece){
    if(piece != nullptr)
        piece->setPos(x(),y());

    //std::cout << "AVANT : " << _piece << std::endl;
    _piece = piece;
    //std::cout << "APRES : " <<_piece << std::endl;
}

void PlateauBox::resetColor(){
    setColor(_color);
}

ChessItem* PlateauBox::getPiece() const{
    return _piece;
}

std::pair<int,int> PlateauBox::getPosition(){
    return _pos;
}

void PlateauBox::movePiece(PlateauBox* box){
    //scene->update();

    setPiece(box->getPiece());
    box->setPiece(nullptr);
    _scene->resetAllColors();

    //box->getPiece()->setPixmapAgain();

}

/*
AbstractClient* PlateauBox::getClient() const{
    return _scene->getClient();
}
*/

void PlateauBox::sendPosition(std::string pos){
    _scene->sendPosition(pos);
}

void PlateauBox::setFog(ChessItem* fog){
    if(fog != nullptr)
        fog->setPos(x(),y());
    _fog = fog;
}

void PlateauBox::removeFog(){
    if(_fog != nullptr)
        delete _fog;
    
    _fog = nullptr;
}

#endif