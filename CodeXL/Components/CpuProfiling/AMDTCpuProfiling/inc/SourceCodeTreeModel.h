//==================================================================================
// Copyright (c) 2013-2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file SourceCodeTreeModel.h
///
//==================================================================================

#ifndef __SOURCECODETREEMODEL_H
#define __SOURCECODETREEMODEL_H

// Qt:
#include <QAbstractItemModel>

// Backend
#include <AMDTCpuProfilingRawData/inc/CpuProfileFunction.h>

// Local:
#include <inc/SourceCodeViewUtils.h>


struct InstOffsetSize
{
    gtVAddr     m_offset = 0;
    gtUInt32    m_size = 0;

    InstOffsetSize(gtVAddr offset, gtUInt32 size) : m_offset(offset), m_size(size) {}
};

class SourceCodeTreeModel : public QAbstractItemModel
{
    friend class SessionSourceCodeView;

public:
    SourceCodeTreeModel(const QString& sessionDir,
                        std::shared_ptr<cxlProfileDataReader> pProfDataRdr,
                        std::shared_ptr<DisplayFilter> displayFilter);

    ~SourceCodeTreeModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    bool insertColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;
    bool removeColumns(int position, int columns, const QModelIndex& parent = QModelIndex()) override;
    bool insertRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex& parent = QModelIndex()) override;

    /// Update the displayed headers:
    bool UpdateHeaders(AMDTUInt32 hotspotCounterId = 0);

    /// Return the index of the top level item parent (or itself), and the index of the item within it's top level item parent:
    bool GetItemTopLevelIndex(const QModelIndex& index, int& indexOfTopLevelItem, int& indexOfTopLevelItemChild) const;

    /// Return the amount of tree top level items:
    int topLevelItemCount() const;

    /// Return the top level item for the requested index:
    SourceViewTreeItem* topLevelItem(int index) const;
    bool isItemTopLevel(SourceViewTreeItem* pItem) const;

    /// Update the model data:
    void UpdateModel();

    /// Return the index for the top level item:
    int indexOfTopLevelItem(SourceViewTreeItem* pItem) const;

    QModelIndex indexOfItem(SourceViewTreeItem* pItem) const;

    /// Fill the current file disassembly lines:
    bool BuildDisassemblyTree();

    /// Add the "double click to view..." item to the tree:
    SourceViewTreeItem* AddDoubleClickItem(SourceViewTreeItem* pAsmItem);

    /// Prepare the tree items for the current file source and dasm lines:
    bool BuildSourceAndDASMTree();

    void SetHotSpotSamples(AMDTUInt32 counterIdx);

    /// Store module details:
    void SetModuleDetails(AMDTUInt32 moduleId, AMDTUInt32 pId);

    AMDTUInt32 GetFuncSrcFirstLnNum() const { return (m_funcFirstSrcLine > 0) ? (m_funcFirstSrcLine - 1) : 0; }
    const std::vector<SourceViewTreeItem*> GetSrcLineViewMap() const { return m_srcLineViewTreeMap; }

private:
    bool SetSourceLines(const QString& filepath, unsigned int startLine, unsigned int stopLine);
    bool SetupSourceInfo();

    SourceViewTreeItem* getItem(const QModelIndex& index) const;
    void SetHotSpotsrcLnSamples(AMDTUInt32 counterId,
                                const AMDTProfileFunctionData&  functionData,
                                const AMDTSourceAndDisasmInfoVec& srcInfoVec);

    void SetHotSpotDisamOnlySamples(AMDTUInt32 counterId,
                                    const AMDTProfileFunctionData&  functionData,
                                    const AMDTSourceAndDisasmInfoVec& srcInfoVec);

    void SetTooltipFormat(double val, double funcPercent, double appPercent, QVariant& tooltip);
    void SetDisplayFormat(double val, bool appendPercent, QVariant& data, const int precision);

    /// Fill the current file source lines:
    void BuildSourceLinesTree(std::vector<SourceViewTreeItem*>& srcLineViewTreeMap);
    void BuildTree(const std::vector<SourceViewTreeItem*>& srcLineViewTreeMap);
    void PopulateFunctionSampleData(const AMDTProfileFunctionData&  functionData,
                                    gtString srcFilePath,
                                    AMDTSourceAndDisasmInfoVec srcInfoVec,
                                    const std::vector<SourceViewTreeItem*>& srcLineViewTreeMap);

    void GetInstOffsets(gtUInt16 srcLine,
                        const AMDTSourceAndDisasmInfoVec& srcInfoVec,
                        gtVector<InstOffsetSize>& instOffsetVec) const;

    void GetDisasmString(gtVAddr offset,
                         const AMDTSourceAndDisasmInfoVec& srcInfoVec,
                         gtString& disasm,
                         gtString& codeByte) const;

    void GetDisasmSampleValue(const InstOffsetSize& instInfo,
                              const AMDTProfileInstructionDataVec& dataVec,
                              AMDTSampleValueVec& sampleValue) const;

private:
    SourceCodeTreeView* m_pSessionSourceCodeTreeView = nullptr;

    bool m_isDisplayingOnlyDasm = false;

    QVector<QString> m_srcLinesCache;
    unsigned int m_startLine = 1;
    unsigned int m_stopLine = 1;

    // Session and module details:
    QString m_sessionDir;
    QString m_moduleName;

    ProcessIdType m_pid = AMDT_PROFILE_ALL_PROCESSES;
    ProcessIdType m_newPid = AMDT_PROFILE_ALL_PROCESSES;
    ThreadIdType m_tid = AMDT_PROFILE_ALL_THREADS;
    ThreadIdType m_newTid = AMDT_PROFILE_ALL_THREADS;
    AMDTFunctionId m_funcId = 0;

    // FIMXE: this mem-var never set, only accessed.
    const CpuProfileFunction* m_pDisplayedFunction = nullptr;

    AMDTModuleType m_modType = AMDT_MODULE_TYPE_NONE;

    bool m_isModuleCached = false;

    // Latest source file to be displayed
    QString m_srcFile;
    // Previous source file (if any) displayed
    QString m_lastSrcFile;

    gtVAddr m_newAddress = 0;

    FuncSymbolsList m_symbolsInfoList;
    FuncSymbolsList::iterator m_currentSymbolIterator;
    FuncSymbolsList::iterator m_lastSymIt;

    // Contain the tree root item:
    SourceViewTreeItem* m_pRootItem = nullptr;

    QStringList m_headerCaptions;
    QStringList m_headerTooltips;

    QColor m_forgroundColor;

    AMDTProfileSourceLineDataVec m_srcLineDataVec;

    std::shared_ptr<cxlProfileDataReader> m_pProfDataRdr;
    std::shared_ptr<DisplayFilter> m_pDisplayFilter;
    AMDTUInt32  m_funcFirstSrcLine = 0;

    // srcLineNumber for sampled  --> SourceViewTreeItem
    std::vector<std::pair<AMDTProfileSourceLineData, SourceViewTreeItem*>> m_sampleSrcLnViewTreeList;
    std::vector<SourceViewTreeItem*> m_srcLineViewTreeMap;
};

#endif //__SOURCECODETREEMODEL_H
