#include "animationviewer.hpp"
#include "ui_animationviewer.h"

#include <float.h>
#include <math.h>

#include <QListView>
#include <QLineEdit>
#include <QSlider>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QCheckBox>

AnimationViewer::AnimationViewer(QAcknextWidget * acknex, MODEL * model, QWidget *parent) :
    QDockWidget(parent),
    acknex(acknex),
    model(model),
    listModel(new ModelAnimationListModel(model)),
    timer(new QTimer()),
    ui(new Ui::AnimationViewer)
{
	ui->setupUi(this);

	ui->list->setModel(this->listModel);

	connect(
		ui->list->selectionModel(),
		&QItemSelectionModel::currentChanged,
		this,
		&AnimationViewer::on_list_currentChanged);

	this->selectAnim(nullptr);

	connect(
		this->timer, &QTimer::timeout,
		this, &AnimationViewer::animateFurther);
	this->timer->setSingleShot(false);
	this->timer->setInterval(25);
}

AnimationViewer::~AnimationViewer()
{
	delete ui;
	delete listModel;
	delete timer;
}


void AnimationViewer::refresh()
{
	ui->list->setModel(nullptr);
	ui->list->setModel(this->listModel);
	connect(
		ui->list->selectionModel(),
		&QItemSelectionModel::currentChanged,
		this,
		&AnimationViewer::on_list_currentChanged);
}

void AnimationViewer::selectAnim(ANIMATION *anim)
{
	bool enabled = (anim != nullptr);

	this->selection = anim;

	ui->name->setEnabled(enabled);
	ui->play->setEnabled(enabled);
	ui->rewind->setEnabled(enabled);
	ui->stop->setEnabled(enabled);
	ui->remove->setEnabled(enabled);
	ui->position->setEnabled(enabled);
	ui->progress->setEnabled(enabled);
	ui->looped->setEnabled(enabled);

	if(enabled) {
		this->timer->stop();
		ui->name->setText(QString(anim->name));
		ui->looped->setChecked(anim->flags & LOOPED);
		ui->progress->setValue(0);
		this->animate(0);
	}
}

void AnimationViewer::animateFurther()
{
	assert(this->selection);
	this->progress += 0.001 * this->timer->interval();

	if(this->progress >= this->selection->duration) {
		if(this->selection->flags & LOOPED) {
			this->progress -= this->selection->duration;
		} else {
			this->progress = this->selection->duration;
			this->timer->stop();
		}
	}

	ui->progress->setValue(100 * this->progress / selection->duration);

	this->animate(this->progress);
}

void AnimationViewer::animate(double frameTime)
{
	assert(this->model);
	assert(this->selection);
	auto * anim = this->selection;

	this->progress = frameTime;

	ent_animate(acknex->entity(), this->selection->name, frameTime);

	engine_log("Animate %s @ %f / %f", anim->name, frameTime, anim->duration);

	double dur = this->selection->duration;
	double pos = this->progress;

	this->ui->position->setText(QString("%1 / %2")
		.arg(pos,4,'f',2)
		.arg(dur,4,'f',2));

	emit this->hasChanged();
}

void AnimationViewer::on_list_currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous);
	this->selectAnim(this->listModel->get(current));
}

void AnimationViewer::on_list_doubleClicked(const QModelIndex &index)
{
	this->selectAnim(this->listModel->get(index));
	this->on_play_clicked();
}

void AnimationViewer::on_progress_valueChanged(int value)
{
	Q_UNUSED(value);
	if(this->timer->isActive() == false) {
		this->animate(this->selection->duration * value / 100.0);
	}
}

void AnimationViewer::on_play_clicked()
{
	this->timer->start();
}

void AnimationViewer::on_stop_clicked()
{
	this->timer->stop();
}

void AnimationViewer::on_rewind_clicked()
{
	this->timer->stop();
    this->progress = 0;
	ui->progress->setValue(0);
}

void AnimationViewer::on_looped_clicked(bool checked)
{
    assert(this->selection);
	this->selection->flags &= ~LOOPED;
	if(checked) {
		this->selection->flags |= LOOPED;
	}
}

void AnimationViewer::closeEvent(QCloseEvent *event)
{
	this->on_stop_clicked();
	QDockWidget::closeEvent(event);
}

void AnimationViewer::on_name_editingFinished()
{
	Q_ASSERT(this->selection != nullptr);

	QByteArray data;
	QString name = ui->name->text();
	do {
		data = name.toUtf8();
		name.chop(1);
	} while(uint(data.size()) >= sizeof(ANIMATION::name));

	memset(
		this->selection->name,
	    0,
		sizeof(BONE::name));
	memcpy(
		this->selection->name,
		data.data(),
		data.size());

	this->listModel->refreshAnimation(this->selection);

	emit this->hasChanged();
}
