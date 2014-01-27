L.Control.Projects = L.Control.extend({
        options: {
                position: 'topleft',
                title: 'Show Recent Projects '
                },

        onAdd: function (map) {
                var className = 'leaflet-control-view-Projects', container;
                
                if(map.zoomControl && !this.options.forceSeparateButton) {
                        container = map.zoomControl._container;
                } else {
                        container = L.DomUtil.create('div', 'leaflet-bar');
                }
                
                this._createButton(this.options, className, container, this.setCenterView, map);

                return container;
        },
        
        _createButton: function (opts, className, container, fn, context) {
                var link = L.DomUtil.create('a', className, container);
                link.href = '#';
                link.title = opts.title;

           
                
                L.DomEvent
                        .addListener(link, 'click', L.DomEvent.stopPropagation)
                        .addListener(link, 'click', L.DomEvent.preventDefault)
                        .addListener(link, 'click',
							function()
							{
                            //context.setView(opts.center, zoom);
							showProjects();
							}
						, context);
                return link;
        }
});

/*
L.Map.addInitHook(function () {
        if (this.options.fullscreenControl) {
                this.fullscreenControl = L.control.fullscreen(this.options.fullscreenControlOptions);
                this.addControl(this.fullscreenControl);
        }
});
*/

L.control.viewcenter = function (options) {
        return new L.Control.ViewCenter(options);
};