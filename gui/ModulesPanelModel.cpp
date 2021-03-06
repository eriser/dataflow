#include "ModulesPanelModel.h"

const Library * at(const LibraryMap * map, unsigned int index)
{
    if (index >= map->size())
        throw std::out_of_range("The given index (" + std::to_string(index) +
                                ") is out of range. Map size = " + std::to_string(map->size()));

    LibraryMap::const_iterator it = map->begin();

    while (it != map->end() && index > 0) {
        index--;
        it++;
    }

    return &it->second;
}

ModulesPanelModel::ModulesPanelModel(ProjectModel * projectModel, QObject * parent) : QAbstractItemModel(parent)
{
    this->rootItem = new ModulesPanelItem("root", projectModel, ModulesPanelItem::RootT);
    loadFromProjectModel(projectModel);
}

ModulesPanelModel::~ModulesPanelModel()
{
    delete rootItem;
}

void ModulesPanelModel::loadFromProjectModel(ProjectModel * projectModel)
{
    beginResetModel();

    const LibraryMap & libraries = projectModel->getLibraries();

    for (const auto & pair : libraries) {
        appendLibrary(pair.second);
    }

    endResetModel();
}

void ModulesPanelModel::addLibrary(const Library & library)
{
    int count = rootItem->childCount();
    beginInsertRows(QModelIndex(), count, count + 1);

    appendLibrary(library);

    endInsertRows();
}

void ModulesPanelModel::removeLibraryAtRow(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    rootItem->removeAt(row);
    endRemoveRows();
}

QModelIndex ModulesPanelModel::index(int row, int column, const QModelIndex & parent) const
{
    ModulesPanelItem * parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ModulesPanelItem *>(parent.internalPointer());

    ModulesPanelItem * childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ModulesPanelModel::parent(const QModelIndex & index) const
{
    if (!index.isValid())
        return QModelIndex();

    auto * childItem = static_cast<ModulesPanelItem *>(index.internalPointer());
    auto * parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ModulesPanelModel::rowCount(const QModelIndex & parent) const
{
    ModulesPanelItem * parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<ModulesPanelItem *>(parent.internalPointer());

    return parentItem->childCount();
}

int ModulesPanelModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
        return static_cast<ModulesPanelItem *>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant ModulesPanelModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    ModulesPanelItem * item = static_cast<ModulesPanelItem *>(index.internalPointer());

    return item->data();
}

Qt::ItemFlags ModulesPanelModel::flags(const QModelIndex & index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsEnabled;
    auto type = static_cast<ModulesPanelItem *>(index.internalPointer())->getItemType();

    switch (type) {
    case ModulesPanelItem::ItemType::OperationT:
    case ModulesPanelItem::ItemType::ConstructorT:
    case ModulesPanelItem::ItemType::LibraryT:
        flags |= Qt::ItemIsSelectable;
        break;

    default:
        break;
    }

    return flags;
}

std::shared_ptr<const Block> ModulesPanelModel::getBlockPtr(const QModelIndex & index) const
{
    if (!index.isValid()) return NULL;

    ModulesPanelItem * item = static_cast<ModulesPanelItem *>(index.internalPointer());
    return std::shared_ptr<const Block>(static_cast<const Block *>(item->getDataPtr()));
}

const Library * ModulesPanelModel::getLibraryPtr(const QModelIndex & index) const
{
    if (!index.isValid()) return NULL;

    auto * item = static_cast<ModulesPanelItem *>(index.internalPointer());

    if (item->parent() != rootItem) return NULL;

    return static_cast<const Library *>(rootItem->child(index.row())->getDataPtr());
}

void ModulesPanelModel::appendLibrary(const Library & library)
{
    auto * libraryItem = rootItem->addLibrary(&library);

    // Block types
    const std::vector<std::string> & operations = library.getOperations();
    const std::vector<std::string> & constructors = library.getTypes();

    // Constructors
    if (constructors.size() > 0) {
        auto * constructorsLabel = libraryItem->addConstructorsLabel();

        for (int i = 0; i < constructors.size(); i++) {
            constructorsLabel->addConstructor(constructors[i]);
        }
    }

    // Operations
    if (operations.size() > 0) {
        auto * operationsLabel = libraryItem->addOperationsLabel();

        for (int i = 0; i < operations.size(); i++) {
            operationsLabel->addOperation(operations[i]);
        }
    }
}

