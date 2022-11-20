
// SameGameView.cpp: реализация класса CSameGameView
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
#ifndef SHARED_HANDLERS
#include "SameGame.h"
#endif

#include "SameGameDoc.h"
#include "SameGameView.h"
#include "OptionDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSameGameView

IMPLEMENT_DYNCREATE(CSameGameView, CView)

BEGIN_MESSAGE_MAP(CSameGameView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()

	ON_COMMAND(ID_LEVEL_3COLORS, &CSameGameView::OnLevel3colors)
	ON_COMMAND(ID_LEVEL_4COLORS, &CSameGameView::OnLevel4colors)
	ON_COMMAND(ID_LEVEL_5COLORS, &CSameGameView::OnLevel5colors)
	ON_COMMAND(ID_LEVEL_6COLORS, &CSameGameView::OnLevel6colors)
	ON_COMMAND(ID_LEVEL_7COLORS, &CSameGameView::OnLevel7colors)
	ON_COMMAND(ID_SETUP_BLOCKCOUNT, &CSameGameView::OnSetupBlockcount)
	ON_COMMAND(ID_SETUP_BLOCKSIZE, &CSameGameView::OnSetupBlocksize)
	ON_COMMAND(ID_EDIT_UNDO, &CSameGameView::OnEditUndo)
	ON_COMMAND(ID_EDIT_PASTE, &CSameGameView::OnEditPaste)

	ON_UPDATE_COMMAND_UI(ID_LEVEL_3COLORS, &CSameGameView::OnUpdateLevel3colors)
	ON_UPDATE_COMMAND_UI(ID_LEVEL_4COLORS, &CSameGameView::OnUpdateLevel4colors)
	ON_UPDATE_COMMAND_UI(ID_LEVEL_5COLORS, &CSameGameView::OnUpdateLevel5colors)
	ON_UPDATE_COMMAND_UI(ID_LEVEL_6COLORS, &CSameGameView::OnUpdateLevel6colors)
	ON_UPDATE_COMMAND_UI(ID_LEVEL_7COLORS, &CSameGameView::OnUpdateLevel7colors)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CSameGameView::OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CSameGameView::OnUpdateEditPaste)
END_MESSAGE_MAP()

// Создание или уничтожение CSameGameView

CSameGameView::CSameGameView() noexcept
{
	// TODO: добавьте код создания

}

CSameGameView::~CSameGameView()
{
}

BOOL CSameGameView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: изменить класс Window или стили посредством изменения
	//  CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// Рисование CSameGameView

void CSameGameView::OnDraw(CDC* pDC)
{
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Сохраняем текущее состояние контекста устройства
	int nDCSave = pDC->SaveDC();

	// Получаем размеры клиентской области
	CRect rcClient;
	GetClientRect(&rcClient);
	COLORREF clr = pDoc->GetBoardSpace(-1, -1);

	// Сначала отрисовываем фон
	pDC->FillSolidRect(&rcClient, clr);

	// Создаем кисть для рисования
	CBrush br;
	br.CreateStockObject(HOLLOW_BRUSH);
	CBrush* pbrOld = pDC->SelectObject(&br);

	// Рисуем блоки
	for (int row = 0; row < pDoc->GetRows(); row++)
	{
		for (int col = 0; col < pDoc->GetColumns(); col++)
		{

			clr = pDoc->GetBoardSpace(row, col);

			// Вычисляем размер и позицию игрового пространства
			CRect rcBlock;
			rcBlock.top = row * pDoc->GetHeight();
			rcBlock.left = col * pDoc->GetWidth();
			rcBlock.right = rcBlock.left + pDoc->GetWidth();
			rcBlock.bottom = rcBlock.top + pDoc->GetHeight();

			// Заполняем блок соответствующим цветом
			pDC->FillSolidRect(&rcBlock, clr);

			// Рисуем контур
			pDC->Rectangle(&rcBlock);
		}
	}
	// Восстанавливаем контекст устройства
	pDC->RestoreDC(nDCSave);
	br.DeleteObject();
}


// Диагностика CSameGameView

#ifdef _DEBUG
void CSameGameView::AssertValid() const
{
	CView::AssertValid();
}

void CSameGameView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSameGameDoc* CSameGameView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSameGameDoc)));
	return (CSameGameDoc*)m_pDocument;
}
#endif //_DEBUG


// Обработчики сообщений CSameGameView


void CSameGameView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	ResizeWindow();
	// TODO: добавьте специализированный код или вызов базового класса
}

void CSameGameView::ResizeWindow()
{
	// Создаем указатель на Document
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Получаем размеры клиентской области
	CRect rcClient, rcWindow;
	GetClientRect(&rcClient);
	GetParentFrame()->GetWindowRect(&rcWindow);
	int nWidthDiff = rcWindow.Width() - rcClient.Width();
	int nHeightDiff = rcWindow.Height() - rcClient.Height();

	// Изменяем размеры окна, исходя из размеров нашей доски
	rcWindow.right = rcWindow.left +
		pDoc->GetWidth() * pDoc->GetColumns() + nWidthDiff;
	rcWindow.bottom = rcWindow.top +
		pDoc->GetHeight() * pDoc->GetRows() + nHeightDiff;

	// Функция MoveWindow() изменяет размер окна фрейма
	GetParentFrame()->MoveWindow(&rcWindow);
}

void CSameGameView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Вначале создаем указатель на Document
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Получаем индекс строки и столбца элемента, по которому был осуществлен клик мышкой
	int row = point.y / pDoc->GetHeight();
	int col = point.x / pDoc->GetWidth();

	// Удаляем блоки из Document
	int count = pDoc->DeleteBlocks(row, col);

	// Проверяем, было ли удаление блоков
	if (count > 0)
	{
		// Перерисовываем View
		Invalidate();
		UpdateWindow();

		// Проверяем, закончилась ли игра
		if (pDoc->IsGameOver())
		{
			// Получаем количество оставшихся блоков
			int remaining = pDoc->GetRemainingCount();
			CString message;
			message.Format(_T("No more moves left\nBlocks remaining: %d"),
				remaining);

			// Отображаем пользователю результат игры
			MessageBox(message, _T("Game Over"), MB_OK | MB_ICONINFORMATION);
		}
	}

	CView::OnLButtonDown(nFlags, point);
}


void CSameGameView::OnLevel3colors()
{
	setColorCount(3);
}


void CSameGameView::OnUpdateLevel3colors(CCmdUI* pCmdUI)
{
	setColorCheck(pCmdUI, 3);
}


void CSameGameView::OnLevel4colors()
{
	setColorCount(4);
}


void CSameGameView::OnUpdateLevel4colors(CCmdUI* pCmdUI)
{
	setColorCheck(pCmdUI, 4);
}


void CSameGameView::OnLevel5colors()
{
	setColorCount(5);
}


void CSameGameView::OnUpdateLevel5colors(CCmdUI* pCmdUI)
{
	setColorCheck(pCmdUI, 5);
}


void CSameGameView::OnLevel6colors()
{
	setColorCount(6);
}


void CSameGameView::OnUpdateLevel6colors(CCmdUI* pCmdUI)
{
	setColorCheck(pCmdUI, 6);
}


void CSameGameView::OnLevel7colors()
{
	setColorCount(7);
}


void CSameGameView::OnUpdateLevel7colors(CCmdUI* pCmdUI)
{
	setColorCheck(pCmdUI, 7);
}

void CSameGameView::setColorCount(int numColors)
{
	// Сначала получаем указатель на документ
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// Устанавливаем количество цветов 
	pDoc->SetNumColors(numColors);
	// Перерисовываем View
	Invalidate();
	UpdateWindow();
}

void CSameGameView::setColorCheck(CCmdUI* pCmdUI, int numColors)
{
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Проверка установленного уровня сложности
	pCmdUI->SetCheck(pDoc->GetNumColors() == numColors);
}


void CSameGameView::OnSetupBlockcount()
{
	// Получаем указатель на Document
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Создаем диалоговое окно
	COptionDialog dlg(true, this);

	// Устанавливаем параметры строк и столбцов
	dlg.m_nValue1 = pDoc->GetRows();
	dlg.m_nValue2 = pDoc->GetColumns();

	// Отображаем полученное окно
	if (dlg.DoModal() == IDOK)
	{
		// Сначала удаляем игровое поле
		pDoc->DeleteBoard();

		// Устанавливаем значения, переданные пользователем
		pDoc->SetRows(dlg.m_nValue1);
		pDoc->SetColumns(dlg.m_nValue2);

		// Обновляем игровое поле
		pDoc->SetupBoard();

		// Изменяем размеры View
		ResizeWindow();
	}
}


void CSameGameView::OnSetupBlocksize()
{
	// Указатель на Document
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Создаем диалоговое окно
	COptionDialog dlg(false, this);

	// Устанавливаем параметры «Ширины/Высоты»
	dlg.m_nValue1 = pDoc->GetWidth();
	dlg.m_nValue2 = pDoc->GetHeight();

	// Отображаем окно
	if (dlg.DoModal() == IDOK)
	{
		// Удаляем игровое поле
		pDoc->DeleteBoard();

		// Считываем введенные пользователем параметры
		pDoc->SetWidth(dlg.m_nValue1);
		pDoc->SetHeight(dlg.m_nValue2);

		// Обновляем игровую доску
		pDoc->SetupBoard();

		// Изменяем размеры View
		ResizeWindow();
	}
}


void CSameGameView::OnEditUndo()
{
	// Получаем указатель на Document
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->UndoLast();

	// Перерисовываем View
	Invalidate();
	UpdateWindow();
}


void CSameGameView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	// Сначала получаем указатель на Document
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Включаем опцию, если она доступна
	pCmdUI->Enable(pDoc->CanUndo());
}


void CSameGameView::OnEditPaste()
{
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	pDoc->RedoLast();

	// Перерисовываем View
	Invalidate();
	UpdateWindow();
}


void CSameGameView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	// Сначала получаем указатель на Document
	CSameGameDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// Включаем опцию, если она доступна
	pCmdUI->Enable(pDoc->CanRedo());
}